/*----------------------Preemptive Scheduling Using Threads PROJECT----------------------*/

//		*************************************************
//		*												*
//		*												*
//		*		Jatin Kesnani				(K21-3204)	*
//		*												*
//		*												*
//		*************************************************

#include <iostream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <iomanip>
#define BOLD_CYAN "\e[1;36m"
#define RESET "\033[0m"

using namespace std;

int sleepcount = 0;
int TimeQuantum;
bool visited[100];
int queue[100], front = 0, rear = 0;
static int index=0;
int n;
int x = 0;
float total_waiting_time = 0;
float total_turnaround_time = 0;
float avg_waiting_time = 0;
float avg_turnaround_time = 0;
int GANTTindex[100], k=0, l=0, GANTTpid[100];
int current_time = 0;
bool check = false;
int anothervar = -1;
int PID[100], PAT[100], PBT[100], PFT[100];
int TAT[100], WT[100];
int RT[100];
sem_t mutex;
sem_t completed;

struct Process{
    int pid;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    int remaining_time;
    int priority;
};

vector<Process> processes;

void Text_Animation(string a){
	for(int i=0 ; a[i]!='\0' ; i++){
		for(long int j=0 ; j<10000000/2; j++){}
		cout<<a[i];
	}
}

bool checkshortest(Process *p)
{
    bool shortest_remaining_time = true;
    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time && i != p->pid - 1) {
            if (processes[i].remaining_time < p->remaining_time)
            {
                shortest_remaining_time = false;
                break;
            }
            else if (processes[i].remaining_time == p->remaining_time && processes[i].arrival_time < p->arrival_time)
            {
                shortest_remaining_time = false;
                break;
            }
        }
    }
    return shortest_remaining_time;
}

bool checklargest(Process *p)
{
    bool largest_remaining_time = true;
    for (int i = 0; i < processes.size(); i++)
	{
        if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time && i != p->pid - 1)
		{
            if (processes[i].remaining_time > p->remaining_time)
            {
                largest_remaining_time = false;
                break;
            }
            else if (processes[i].remaining_time == p->remaining_time && processes[i].arrival_time < p->arrival_time)
            {
                largest_remaining_time = false;
                break;
            }
        }
    }
    return largest_remaining_time;
}

bool check_process_arrival()
{
    bool arrived = false;
    for (int i = 0; i < processes.size(); i++)
	{
        if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time)
        {
            arrived = true;
            break;
        }
    }
    return arrived;
}

void* srtf_algorithm(void* arg)
{
    Process* process = (Process*) arg;
    cout << "Process: P" << process->pid << " has arrived! thread created for P" << process->pid << "\n";
    process->remaining_time = process->burst_time;
    sleepcount--;
    sleep(sleepcount);
    
    
	while (process->remaining_time > 0)
    {	
        if(process->arrival_time <= current_time)
        {
            sem_wait(&mutex);
            bool flag = checkshortest(process);
            if(flag == true)
            {
                process->remaining_time--;
                current_time++;
				process->completion_time = current_time;
            }
            sem_post(&mutex);
        }
        else
		{
           if(!check_process_arrival())
		   {
           		sem_wait(&mutex);
                current_time++;
                sem_post(&mutex);
           }
        }
    }
	
	process->turnaround_time = process->completion_time - process->arrival_time;
    process->waiting_time = process->turnaround_time - process->burst_time;
    
	sem_post(&completed);
    	return NULL;
}

void* lrtf_algorithm(void* arg)
{
    Process* process = (Process*) arg;
    cout << "Process: P" << process->pid << " has arrived! thread created for P" << process->pid << "\n";
    process->remaining_time = process->burst_time;
    sleepcount--;
    sleep(sleepcount);
    while (process->remaining_time > 0)
    {
        if(process->arrival_time <= current_time)
        {
            sem_wait(&mutex);
            bool flag = checklargest(process);
            if(flag == true)
            {
                process->remaining_time--;
                current_time++;
                process->completion_time = current_time;
            }
            sem_post(&mutex);
        }
        else
		{
           if(!check_process_arrival())
		   {
           		sem_wait(&mutex);
                current_time++;
                sem_post(&mutex);
           }
        }
    }
 
    process->turnaround_time = process->completion_time - process->arrival_time;
    process->waiting_time = process->turnaround_time - process->burst_time;

    sem_post(&completed);
    return NULL;
}

void* priority_algorithm(void* arg)
{
    Process* process = (Process*) arg;
    cout << "Process: P" << process->pid << " has arrived! thread created for P" << process->pid << "\n";
    process->remaining_time = process->burst_time;
    sleepcount--;
    sleep(sleepcount);
    
    while(1){
    	int HPPIndex = -1;
    	int HPP = 2147483647;
    	bool iscompleted = true;
    	for(int i=0 ; i<n ; i++){
    		if(processes[i].remaining_time > 0){
    			iscompleted = false;
                if(processes[i].arrival_time <= current_time){
                	sem_wait(&mutex);
                    if(processes[i].priority < HPP){
                        HPP = processes[i].priority;
                        HPPIndex = i;
                        check = true;
                    }
                    sem_post(&mutex);
                }
    		}
    	}
    	if(iscompleted){
    		break;
		}
    	if(check == false){
    		sem_wait(&mutex);
            current_time++;
            if(GANTTpid[l-1] == -1){
            	GANTTindex[k-1] = current_time;
			}
			else{
				GANTTindex[k] = current_time;
		        k++;
		        GANTTpid[l] = -1;
		        l++;
			}
            sem_post(&mutex);
            continue;
        }
    	processes[HPPIndex].remaining_time--;
        current_time++;
		if(processes[HPPIndex].remaining_time == 0){
    		check = false;
			processes[HPPIndex].completion_time = current_time;
			processes[HPPIndex].turnaround_time = processes[HPPIndex].completion_time - processes[HPPIndex].arrival_time;
	        processes[HPPIndex].waiting_time = processes[HPPIndex].turnaround_time - processes[HPPIndex].burst_time;
			GANTTindex[k] = current_time;
    		k++;
    		GANTTpid[l] = processes[HPPIndex].pid;
    		l++;
		}
		if(anothervar != HPPIndex && processes[anothervar].remaining_time != 0){
    		if(anothervar != -1){
				GANTTindex[k] = current_time-1;
				k++;
    			GANTTpid[l] = processes[anothervar].pid;
				l++;	
			}
		}
        anothervar = HPPIndex;
        sem_post(&completed);
	}
    return NULL;
}

void* rr_algorithm(void* arg)
{
    Process* process = (Process*) arg;
    cout << "Process: P" << process->pid << " has arrived! thread created.\n";
    process->remaining_time = process->burst_time;
    sleepcount--;
    sleep(sleepcount);
    while (process->remaining_time > 0){
	    index = queue[front];
	    front++;
	    if(processes[index].remaining_time == processes[index].burst_time){
	  		if(current_time < processes[index].arrival_time){
	  			sem_wait(&mutex);
	  			current_time = processes[index].arrival_time;
	  			GANTTindex[k] = current_time;
		        k++;
		        GANTTpid[l] = -1;
		        l++;
	  			sem_post(&mutex);
			}	
		}
		if(processes[index].remaining_time-TimeQuantum > 0){
			sem_wait(&mutex);
	        processes[index].remaining_time -= TimeQuantum;
	        current_time += TimeQuantum;
	        GANTTindex[k] = current_time;
	        k++;
	        GANTTpid[l] = processes[index].pid;
	        l++;
	        sem_post(&mutex);
	    }
	    else{
			current_time += processes[index].remaining_time;
			GANTTindex[k] = current_time;
	        k++;
	        GANTTpid[l] = processes[index].pid;
	        l++;
			processes[index].remaining_time = 0;
			processes[index].completion_time = current_time;
			processes[index].turnaround_time = processes[index].completion_time - processes[index].arrival_time;
			processes[index].waiting_time = processes[index].turnaround_time - processes[index].burst_time;
			
		    sem_post(&completed);	
		}
		for(int i=1 ; i<n ; i++){
	    	if(processes[i].remaining_time>0 && processes[i].arrival_time<=current_time && visited[i]==false){
	    		queue[++rear] = i;
	    		visited[i] = true;
	    	}
		}
		if(processes[index].remaining_time>0){
	    	queue[++rear] = index;
		}
		if(front>rear){
	      	for(int i=1 ; i<n ; i++){
	        	if(processes[i].remaining_time>0){
	          		queue[++rear] = i;
	          		visited[i] = true;
	        		break;
	    		}
	    	}
	  	}	 
	}
    return NULL;
}

void SRTFGanttChart(){
	int Complete = 0, Current_Time = 0, minimum = 9999999;
    int Smallest = 0;
    bool Check = false;
	int Anothervar = -1;
	
	GANTTindex[k] = Current_Time;
	k++;
	while(Complete != n){
        for(int j=0 ; j<n ; j++){
            if((PAT[j]<=Current_Time) && (RT[j]<minimum) && (RT[j] > 0)){
                minimum = RT[j];
                Smallest = j;
                Check = true;
            }
        }
        if(Check == false){
            Current_Time++;				
        	if(GANTTpid[l-1] == -1){
            	GANTTindex[k-1] = Current_Time;
			}
			else{
				GANTTindex[k] = Current_Time;
		        k++;
		        GANTTpid[l] = -1;
		        l++;
			}
            continue;
        }
        RT[Smallest]--;
        minimum = RT[Smallest];
        if(minimum == 0){
            minimum = 9999999;
		}
        if(RT[Smallest] == 0){
            Complete++;
            Check = false;
            PFT[Smallest] = Current_Time + 1;
            GANTTindex[k] = PFT[Smallest];		
			k++;
    		GANTTpid[l] = PID[Smallest];
			l++;	
        }
        Current_Time++;
        if(Anothervar != Smallest && RT[Anothervar] != 0){
    		if(Anothervar != -1){
	    		GANTTindex[k] = Current_Time-1;
				k++;
    			GANTTpid[l] = PID[Anothervar];
				l++;	
			}
		}
        Anothervar = Smallest;
    }
}

void LRTFGanttChart(){
	int Complete = 0, Current_Time = 0, maximum = -9999999;
    int Smallest = 0;
    bool Check = false;
	int Anothervar = -1;
	
	GANTTindex[k] = Current_Time;
	k++;
	while(Complete != n){
        for(int j=0 ; j<n ; j++){
            if((PAT[j]<=Current_Time) && (RT[j]>maximum) && (RT[j] > 0)){
                maximum = RT[j];
                Smallest = j;
                Check = true;
            }
        }
        if(Check == false){
            Current_Time++;				
        	if(GANTTpid[l-1] == -1){
            	GANTTindex[k-1] = Current_Time;
			}
			else{
				GANTTindex[k] = Current_Time;
		        k++;
		        GANTTpid[l] = -1;
		        l++;
			}
            continue;
        }
        RT[Smallest]--;
        maximum = RT[Smallest];
        if(maximum == 0){
            maximum = -9999999;
		}
        if(RT[Smallest] == 0){
            Complete++;
            Check = false;
            PFT[Smallest] = Current_Time + 1;
            GANTTindex[k] = PFT[Smallest];		
			k++;
    		GANTTpid[l] = PID[Smallest];
			l++;	
        }
        Current_Time++;
        if(Anothervar != Smallest && RT[Anothervar] != 0){
    		if(Anothervar != -1){
	    		GANTTindex[k] = Current_Time-1;
				k++;
    			GANTTpid[l] = PID[Anothervar];
				l++;	
			}
		}
        Anothervar = Smallest;
    }
}

int main(){
	cout << BOLD_CYAN;
	cout<<"\n\n\n\n\n";
	Text_Animation("\t\t\t\t _________________________________________________________________________\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|    ~~~~~~~~~ Project : Preemptive Scheduling Using Threads ~~~~~~~~~    |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|_________________________________________________________________________|\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|                     Prepared By : Jatin Kesnani                         |\n");
	Text_Animation("\t\t\t\t|                                                                         |\n");
	Text_Animation("\t\t\t\t|_________________________________________________________________________|\n");
	cout<<"\n\t\t\t\t\t";
	
	cout << RESET;
	cout << "\n---------------------------------------------------------\n";
	cout << "|\t\tSELECT PREEMPTIVE SCHEDULING ALGORITHM\t|\n";
	cout << "---------------------------------------------------------\n";
	cout << "|\t\t[1]	SRTF\t\t\t\t|\n";
	cout << "|\t\t[2]	PRIORITY\t\t\t|\n";
	cout << "|\t\t[3]	LRTF\t\t\t\t|\n";
	cout << "|\t\t[4]	ROUND ROBIN\t\t\t|\n";
	cout << "|-------------------------------------------------------|\n";
	cout << "\n\t\tCHOOSE: ";
	cin >> x;
	
	while(x < 1 || x > 4)
	{
		cout << "\t\tInvalid input!\n";
		cout << "\t\tCHOOSE AGAIN: ";
		cin >> x;
	}
	
	if(x == 1)
	{
		cout << "\nEnter the Number of Processes: ";
	    cin >> n;
	    sleepcount = n;
	    sem_init(&mutex, 0, 1);
	    sem_init(&completed, 0, 0);
	
	    for (int i = 0; i < n; i++)
		{
	        Process p;
	        p.pid = i + 1;
	        PID[i] = p.pid;
	        cout << "Enter Arrival Time and Burst Time for Process " << i + 1 << ": ";
	        cin >> p.arrival_time >> p.burst_time;
	        p.remaining_time = p.burst_time;
	        PAT[i] = p.arrival_time;
	        PBT[i] = p.burst_time;
	        RT[i] = p.remaining_time;
	        processes.push_back(p);
	    }
	    
	    cout<<endl;
	    cout<<"-----------------------------"<<endl;
		cout<<"| Process | Arrival | Burst |"<<endl;
		cout<<"|   ID    |  Time   | Time  |"<<endl;
		cout<<"-----------------------------"<<endl;
		for(int i=0 ; i<n ; i++){
			if(processes[i].pid >= 10){
				cout<<"|   P"<<processes[i].pid<<"   |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<endl;
			}
			else{
	    		cout<<"|   P"<<processes[i].pid<<"    |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<endl;
			}
	    }
	    cout<<"-----------------------------"<<endl;
	    
	    pthread_t thread_id[n];
	    cout << endl;
	    for (int i = 0; i < n; i++)
		{
	        pthread_create(&thread_id[i], NULL, srtf_algorithm, (void*) &processes[i]);
	    }
	
	    int completed_processes = 0;
	    while (completed_processes < n)
		{
	        sem_wait(&completed);
	        completed_processes++;
	    }
	    
	    sem_destroy(&mutex);
	    sem_destroy(&completed);
	    
	    cout<<endl;
	    cout<<"----------------------------------------------------------------"<<endl;
		cout<<"| Process | Arrival | Burst | Finishing | Turnaround | Waiting |"<<endl;
		cout<<"|   ID    |  Time   | Time  |   Time    |    Time    |  Time   |"<<endl;
		cout<<"----------------------------------------------------------------"<<endl;
	    for (int i = 0; i < n; i++) {
			if(processes[i].pid >= 10){
				cout<<"|   P"<<processes[i].pid<<"   |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(6)<<processes[i].completion_time<<"     |"<<setw(7)<<processes[i].turnaround_time<<"     |"<<setw(5)<<processes[i].waiting_time<<"    |"<<endl;
			}
			else{
				cout<<"|   P"<<processes[i].pid<<"    |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(6)<<processes[i].completion_time<<"     |"<<setw(7)<<processes[i].turnaround_time<<"     |"<<setw(5)<<processes[i].waiting_time<<"    |"<<endl;
			}
	        total_waiting_time += processes[i].waiting_time;
	        total_turnaround_time += processes[i].turnaround_time;
	    }
		cout<<"----------------------------------------------------------------"<<endl;
		
	    avg_waiting_time = total_waiting_time / n;
	    avg_turnaround_time = total_turnaround_time / n;
	    
	    cout<<endl;
	    cout << "Average Waiting Time    : " << avg_waiting_time << endl;
	    cout << "Average Turnaround Time : " << avg_turnaround_time << endl;
	    
	    SRTFGanttChart();
	    
	    for(int i=0 ; i<n-1; i++){
	    	for(int j=i+1 ; j<n; j++){
				if(processes[j].arrival_time < processes[i].arrival_time){
					Process Temp = processes[i];
					processes[i] = processes[j];
					processes[j] = Temp;
				}
				else if(processes[j].arrival_time == processes[i].arrival_time){
					if(processes[j].pid < processes[i].pid){
						Process Temp = processes[i];
						processes[i] = processes[j];
						processes[j] = Temp;
					}
				}
			}
		}
		
	    cout<<endl;
		cout<<"\t\t--> GANTT Chart <--\n ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		cout<<" |";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] == -1){
				cout<<" ~~ |";
			}
			else{
				cout<<" P"<<GANTTpid[i]<<" |";
			}
		}
		cout<<endl;
		cout<<" ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		for(int i=0 ; i<k ; i++){
			if(i==0){
				cout<<" "<<GANTTindex[i];
			}
			else if(GANTTpid[i-1] >= 10){
				cout<<setw(6)<<GANTTindex[i];
			}
			else{
				cout<<setw(5)<<GANTTindex[i];
			}
		}
		cout<<endl;
	}
	if(x == 2)
	{
		cout << "\nEnter the Number of Processes: ";
	    cin >> n;
	    sleepcount = n;
	    sem_init(&mutex, 0, 1);
	    sem_init(&completed, 0, 0);
	
	    for (int i = 0; i < n; i++)
	    {
	        Process p;
	        p.pid = i + 1;
	        cout << "Enter Arrival Time, Burst Time and Priority for Process " << i + 1 << ": ";
	        cin >> p.arrival_time >> p.burst_time >> p.priority;
	        p.remaining_time = p.burst_time;
	        processes.push_back(p);
	    }
	    
	    cout<<endl;
	    cout<<"----------------------------------------"<<endl;
		cout<<"| Process | Arrival | Burst | Process  |"<<endl;
		cout<<"|   ID    |  Time   | Time  | Priority |"<<endl;
		cout<<"----------------------------------------"<<endl;
		for(int i=0 ; i<n ; i++){
			if(processes[i].pid >= 10){
				cout<<"|   P"<<processes[i].pid<<"   |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(6)<<processes[i].priority<<"    |"<<endl;
			}
			else{
	    		cout<<"|   P"<<processes[i].pid<<"    |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(6)<<processes[i].priority<<"    |"<<endl;
			}
	    }
	    cout<<"----------------------------------------"<<endl;
	    
	    
	    cout << endl;
	    GANTTindex[k] = current_time;
		k++;
	    pthread_t thread_id[n];
	    
	    for (int i = 0; i < n; i++)
	    {
	        pthread_create(&thread_id[i], NULL, priority_algorithm, (void*) &processes[i]);
	    }
	
	    int completed_processes = 0;
	    while (completed_processes < n)
	    {
	        sem_wait(&completed);
	        completed_processes++;
	    }
	
	    sem_destroy(&mutex);
	    sem_destroy(&completed);
	    cout<<endl;
	    cout<<"---------------------------------------------------------------------------"<<endl;
		cout<<"| Process | Arrival | Burst | Process  | Finishing | Turnaround | Waiting |"<<endl;
		cout<<"|   ID    |  Time   | Time  | Priority |   Time    |    Time    |  Time   |"<<endl;
		cout<<"---------------------------------------------------------------------------"<<endl;
	    for (int i = 0; i < n; i++) {
			if(processes[i].pid >= 10){
				cout<<"|   P"<<processes[i].pid<<"   |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(5)<<processes[i].priority<<"     |"<<setw(6)<<processes[i].completion_time<<"     |"<<setw(7)<<processes[i].turnaround_time<<"     |"<<setw(5)<<processes[i].waiting_time<<"    |"<<endl;
			}
			else{
				cout<<"|   P"<<processes[i].pid<<"    |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(5)<<processes[i].priority<<"     |"<<setw(6)<<processes[i].completion_time<<"     |"<<setw(7)<<processes[i].turnaround_time<<"     |"<<setw(5)<<processes[i].waiting_time<<"    |"<<endl;
			}
	        total_waiting_time += processes[i].waiting_time;
	        total_turnaround_time += processes[i].turnaround_time;
	    }
		cout<<"---------------------------------------------------------------------------"<<endl;
		
	    avg_waiting_time = total_waiting_time / n;
	    avg_turnaround_time = total_turnaround_time / n;
	    
	    cout<<endl;
	    cout << "Average Waiting Time    : " << avg_waiting_time << endl;
	    cout << "Average Turnaround Time : " << avg_turnaround_time << endl;
	    
	    cout<<endl;
		cout<<"\t\t--> GANTT Chart <--\n ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		cout<<" |";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] == -1){
				cout<<" ~~ |";
			}
			else{
				cout<<" P"<<GANTTpid[i]<<" |";
			}
		}
		cout<<endl;
		cout<<" ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		for(int i=0 ; i<k ; i++){
			if(i==0){
				cout<<" "<<GANTTindex[i];
			}
			else if(GANTTpid[i-1] >= 10){
				cout<<setw(6)<<GANTTindex[i];
			}
			else{
				cout<<setw(5)<<GANTTindex[i];
			}
		}
		cout<<endl;
	}
	if(x == 3)
	{
		cout << "\nEnter the Number of Processes: ";
	    cin >> n;
	    sleepcount = n;
	    sem_init(&mutex, 0, 1);
	    sem_init(&completed, 0, 0);
	
	    for (int i = 0; i < n; i++)
		{
	        Process p;
	        p.pid = i + 1;
	        PID[i] = p.pid;
	        cout << "Enter Arrival Time and Burst Time for Process " << i + 1 << ": ";
	        cin >> p.arrival_time >> p.burst_time;
	        p.remaining_time = p.burst_time;
	        PAT[i] = p.arrival_time;
	        PBT[i] = p.burst_time;
	        RT[i] = p.remaining_time;
	        processes.push_back(p);
		}
	    
	    cout<<endl;
	    cout<<"-----------------------------"<<endl;
		cout<<"| Process | Arrival | Burst |"<<endl;
		cout<<"|   ID    |  Time   | Time  |"<<endl;
		cout<<"-----------------------------"<<endl;
		for(int i=0 ; i<n ; i++){
			if(processes[i].pid >= 10){
				cout<<"|   P"<<processes[i].pid<<"   |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<endl;
			}
			else{
	    		cout<<"|   P"<<processes[i].pid<<"    |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<endl;
			}
	    }
	    cout<<"-----------------------------"<<endl;
	    
	   
	    pthread_t thread_id[n];
	    cout << endl;
	    for (int i = 0; i < n; i++)
		{
	        pthread_create(&thread_id[i], NULL, lrtf_algorithm, (void*) &processes[i]);
	    }
	
	    int completed_processes = 0;
	    while (completed_processes < n)
		{
	        sem_wait(&completed);
	        completed_processes++;
	    }
	    
	    sem_destroy(&mutex);
	    sem_destroy(&completed);
	    
	    cout<<endl;
	    cout<<"----------------------------------------------------------------"<<endl;
		cout<<"| Process | Arrival | Burst | Finishing | Turnaround | Waiting |"<<endl;
		cout<<"|   ID    |  Time   | Time  |   Time    |    Time    |  Time   |"<<endl;
		cout<<"----------------------------------------------------------------"<<endl;
	    for (int i = 0; i < n; i++) {
			if(processes[i].pid >= 10){
				cout<<"|   P"<<processes[i].pid<<"   |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(6)<<processes[i].completion_time<<"     |"<<setw(7)<<processes[i].turnaround_time<<"     |"<<setw(5)<<processes[i].waiting_time<<"    |"<<endl;
			}
			else{
				cout<<"|   P"<<processes[i].pid<<"    |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(6)<<processes[i].completion_time<<"     |"<<setw(7)<<processes[i].turnaround_time<<"     |"<<setw(5)<<processes[i].waiting_time<<"    |"<<endl;
			}
	        total_waiting_time += processes[i].waiting_time;
	        total_turnaround_time += processes[i].turnaround_time;
	    }
		cout<<"----------------------------------------------------------------"<<endl;
		
	    avg_waiting_time = total_waiting_time / n;
	    avg_turnaround_time = total_turnaround_time / n;
	    
	    cout<<endl;
	    cout << "Average Waiting Time    : " << avg_waiting_time << endl;
	    cout << "Average Turnaround Time : " << avg_turnaround_time << endl;
	    
	    LRTFGanttChart();
	    
	    for(int i=0 ; i<n-1; i++){
	    	for(int j=i+1 ; j<n; j++){
				if(processes[j].arrival_time < processes[i].arrival_time){
					Process Temp = processes[i];
					processes[i] = processes[j];
					processes[j] = Temp;
				}
				else if(processes[j].arrival_time == processes[i].arrival_time){
					if(processes[j].pid < processes[i].pid){
						Process Temp = processes[i];
						processes[i] = processes[j];
						processes[j] = Temp;
					}
				}
			}
		}
		
	    cout<<endl;
		cout<<"\t\t--> GANTT Chart <--\n ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		cout<<" |";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] == -1){
				cout<<" ~~ |";
			}
			else{
				cout<<" P"<<GANTTpid[i]<<" |";
			}
		}
		cout<<endl;
		cout<<" ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		for(int i=0 ; i<k ; i++){
			if(i==0){
				cout<<" "<<GANTTindex[i];
			}
			else if(GANTTpid[i-1] >= 10){
				cout<<setw(6)<<GANTTindex[i];
			}
			else{
				cout<<setw(5)<<GANTTindex[i];
			}
		}
		cout<<endl;
	}
    if(x == 4){
    	cout << "Enter the Number of Processes: ";
	    cin >> n;
		cout << "Enter Time Quantum: ";
	    cin >> TimeQuantum;
	    sleepcount = n;
	    sem_init(&mutex, 0, 1);
	    sem_init(&completed, 0, 0);
	    
	    
		for (int i = 0; i < n; i++) {
	        Process p;
	        p.pid = i + 1;
	        cout << "Enter Arrival Time and Burst Time for Process " << i + 1 << ": ";
	        cin >> p.arrival_time >> p.burst_time;
	        p.remaining_time = p.burst_time;
	        processes.push_back(p);
	        visited[i] = false;
	    }
	    
	    cout<<endl;
	    cout<<"-----------------------------"<<endl;
		cout<<"| Process | Arrival | Burst |"<<endl;
		cout<<"|   ID    |  Time   | Time  |"<<endl;
		cout<<"-----------------------------"<<endl;
		for(int i=0 ; i<n ; i++){
			if(processes[i].pid >= 10){
				cout<<"|   P"<<processes[i].pid<<"   |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<endl;
			}
			else{
	    		cout<<"|   P"<<processes[i].pid<<"    |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<endl;
			}
	    }
	    cout<<"-----------------------------"<<endl;
	    
	    for(int i=0 ; i<n-1; i++){
	    	for(int j=i+1 ; j<n; j++){
				if(processes[j].arrival_time < processes[i].arrival_time){
					Process Temp = processes[i];
					processes[i] = processes[j];
					processes[j] = Temp;
				}
				else if(processes[j].arrival_time == processes[i].arrival_time){
					if(processes[j].pid < processes[i].pid){
						Process Temp = processes[i];
						processes[i] = processes[j];
						processes[j] = Temp;
					}
				}
			}
		}
	   	queue[0] = 0;
	   	visited[0] = true;
	   	GANTTindex[k] = current_time;
		k++;
	    pthread_t thread_id[n];
	    for (int i = 0; i < n; i++) {
	        pthread_create(&thread_id[i], NULL, rr_algorithm, (void*) &processes[i]);
	    }
	
	    int completed_processes = 0;
	    while (completed_processes < n) {
	        sem_wait(&completed);
	        completed_processes++;
	    }
	
	    sem_destroy(&mutex);
	    sem_destroy(&completed);
	    
		cout<<endl;
	    cout<<"----------------------------------------------------------------"<<endl;
		cout<<"| Process | Arrival | Burst | Finishing | Turnaround | Waiting |"<<endl;
		cout<<"|   ID    |  Time   | Time  |   Time    |    Time    |  Time   |"<<endl;
		cout<<"----------------------------------------------------------------"<<endl;
	    for (int i = 0; i < n; i++) {
			if(processes[i].pid >= 10){
				cout<<"|   P"<<processes[i].pid<<"   |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(6)<<processes[i].completion_time<<"     |"<<setw(7)<<processes[i].turnaround_time<<"     |"<<setw(5)<<processes[i].waiting_time<<"    |"<<endl;
			}
			else{
				cout<<"|   P"<<processes[i].pid<<"    |"<<setw(5)<<processes[i].arrival_time<<"    |"<<setw(4)<<processes[i].burst_time<<"   |"<<setw(6)<<processes[i].completion_time<<"     |"<<setw(7)<<processes[i].turnaround_time<<"     |"<<setw(5)<<processes[i].waiting_time<<"    |"<<endl;
			}
	        total_waiting_time += processes[i].waiting_time;
	        total_turnaround_time += processes[i].turnaround_time;
	    }
		cout<<"----------------------------------------------------------------"<<endl;
		
	    avg_waiting_time = total_waiting_time / n;
	    avg_turnaround_time = total_turnaround_time / n;
	    
	    cout<<endl;
	    cout << "Average Waiting Time    : " << avg_waiting_time << endl;
	    cout << "Average Turnaround Time : " << avg_turnaround_time << endl;
		
		cout<<endl;
		cout<<"\t\t--> GANTT Chart <--\n ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		cout<<" |";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] == -1){
				cout<<" ~~ |";
			}
			else{
				cout<<" P"<<GANTTpid[i]<<" |";
			}
		}
		cout<<endl;
		cout<<" ";
		for(int i=0 ; i<l ; i++){
			if(GANTTpid[i] >= 10 || i==l-1){
				cout<<"------";
			}
			else{
				cout<<"-----";
			}
		}
		cout<<endl;
		for(int i=0 ; i<k ; i++){
			if(i==0){
				cout<<" "<<GANTTindex[i];
			}
			else if(GANTTpid[i-1] >= 10){
				cout<<setw(6)<<GANTTindex[i];
			}
			else{
				cout<<setw(5)<<GANTTindex[i];
			}
		}
		cout<<endl;
	}
	cout<<endl;
    return 0;
}