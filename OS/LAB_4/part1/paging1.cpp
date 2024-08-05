#include<bits/stdc++.h>
using namespace std;


 int PAGE_SIZE;  // Page size in bytes
 int MAX_MM_SIZE ; // Maximum MM SIZE in main memory
 int MAX_VM_SIZE ;
 int curr_mm_size;
 int curr_vm_size;

struct list_commands
{
   string name;
   int arg1;
   int arg2;
   int arg3;
};
// To store the list of commands for each input file.
class Process {
    public:
    int pid;
    int size;
    vector<list_commands>commands;
    vector<int>PFNs;
    vector<bool>PFNs_ocp;
    map<int,int>PT;
    vector<int>PA;
    vector<int>VA;
    map<int,int>map1;
    bool is_in_MM;
};
// Created a class process which stores the contents of the process.
map<int,Process*>proc;

vector<int>LRU;
int processpid=1;



int main(int argc, char * argv[])
 {
    string filen = argv[8];
    string filen1= filen + ".txt";
    ifstream inputFile(filen1);
    string filen3 = argv[10];
    string filen4 = filen3 +".txt";
    ofstream outputFile(filen4);
    MAX_MM_SIZE  = atoi(argv[2])*1024;
    MAX_VM_SIZE = atoi(argv[4])*1024;
    PAGE_SIZE = atoi(argv[6]);
    vector<int>values(MAX_MM_SIZE,0);
    vector<int>values_v(MAX_VM_SIZE,0);
    vector<bool>is_empty_v((MAX_VM_SIZE/PAGE_SIZE),false);
    vector<Process*> mainM;
    vector<Process*> virtualM;
    vector<bool>is_empty((MAX_MM_SIZE/PAGE_SIZE),false);
    curr_mm_size=MAX_MM_SIZE;
    curr_vm_size=MAX_VM_SIZE;
    string command;
    while( getline(inputFile,command) )
    {
        vector<string>words;
        stringstream toDivide(command);
        string data;
        while(getline(toDivide,data,' '))
        {
            words.push_back(data);  
        }
        if(words[0]=="load")
        {
            for(int i=1;i<words.size();i++)
            {
                string filename = words[i] + ".txt" ;
                Process * process = new Process;
                
                process->pid =processpid;
                proc[process->pid]=process;
                processpid++;
                 ifstream inputFile(filename);
                if (!inputFile.is_open()) 
                {
                outputFile<< "could not be loaded - file does not exist" << endl;
                continue;
                }
                string comm; int count =0;
                while( getline(inputFile,comm))
                {
                    vector<string>words1;
                    stringstream toDivide(comm);
                    string data1;
                    while(getline(toDivide,data1,' '))
                    {
                        words1.push_back(data1);    
                    }
                    if(count==0)
                    {
                        int size = stoi(words1[0]);
                        process->size = size*1024;
                        count++;
                    }
                    else
                    {
                        list_commands list;
                        for(int i1=0;i1<words1.size();i1++)
                        {
                            string str = words1[i1];
                            if(i1==0)
                            {
                                list.name = str;
                            }
                            else if(i1==1)
                            {
                                int arg1= stoi(str);
                                list.arg1=arg1;
                            }
                            else if(i1==2)
                            {
                                int arg2= stoi(str);
                                list.arg2=arg2;
                            }
                            else if(i1==3)
                            {
                                int arg3= stoi(str);
                                list.arg3=arg3;
                            }
                        }
                        process->commands.push_back(list);
                    }
                }
                int siz = process->size;
                if(curr_mm_size >= siz)
                {
                    mainM.push_back(process);
                    process->is_in_MM = true;
                    curr_mm_size-=siz;
                    outputFile<<"filename"<<process->pid<<" is loaded in main memory and is assigned process id "<<process->pid<<endl;
                    int VPN1=0;
                    for(int ii=0;ii<is_empty.size();ii++)
                    {
                        if(process->PFNs.size()==siz/PAGE_SIZE) break;
                        if(is_empty[ii]==false)
                        {
                            is_empty[ii]=true;
                            process->PFNs.push_back(ii);
                            process->PT[VPN1] = ii;
                            VPN1++;
                            //cout<<ii<<" ";
                            process->PFNs_ocp.push_back(false);
                        }
                    }
                    //cout<<endl;
                    LRU.push_back(process->pid);
                }
                else if(curr_vm_size >= siz)
                {
                    virtualM.push_back(process);
                    process->is_in_MM = false;
                    curr_vm_size-=siz;
                    outputFile<<"filename"<<process->pid<<" is loaded in virtual memory and is assigned process id "<<process->pid<<endl;
                }
                else
                {
                    outputFile<<"could not be loaded - memory is full"<<endl;
                }
            }
        }
        else if(words[0]=="run")
        {
            //cout<<proc.size()<<endl;
            string num = words[1];
            int proc_id= stoi(num);
            if(proc.find(proc_id)==proc.end())
            {
                outputFile<<"Process doesnt exist"<<endl;
                continue;
            }
            Process * process = proc[proc_id];
            if(!(process->is_in_MM))
            {
                vector<Process*>v2;
                for(int ii2=0;ii2<virtualM.size();ii2++)
                {
                    if(virtualM[ii2]->pid!=proc_id)
                    {
                        v2.push_back(virtualM[ii2]);
                    }
                }
                virtualM=v2;
                curr_vm_size+=process->size;
                int siz = process->size;
                if(curr_mm_size>=siz)
                {

                    mainM.push_back(process);
                    process->is_in_MM = true;
                    curr_mm_size-=siz;
                    int VPN1=0;
                    for(int ii=0;ii<is_empty.size();ii++)
                    {
                        if(process->PFNs.size()==siz/PAGE_SIZE) break;
                        if(is_empty[ii]==false)
                        {
                            is_empty[ii]=true;
                            process->PFNs.push_back(ii);
                            process->PT[VPN1]=ii;
                            VPN1++;
                            process->PFNs_ocp.push_back(false);
                        }
                    }
                }
                else
                {
                   while(siz>0)
                   {
                        int pid = LRU[0];
                        Process* old_p = proc[pid];
                        curr_mm_size+=old_p->size;
                        virtualM.push_back(old_p);
                        for(int i8=0;i8<old_p->PA.size();i8++)
                        {
                            values[old_p->PA[i8]]=0;
                        }
                        old_p->PA.clear();
                        old_p->is_in_MM = false;
                        for(int ik=0;ik<old_p->PFNs.size();ik++)
                        {
                            is_empty[old_p->PFNs[ik]]=false;
                        }
                        old_p->PFNs.clear();
                        old_p->PFNs_ocp.clear();
                        old_p->PT.clear();
                        siz-=old_p->size;
                        vector<int>LRU1;
                        for(int ji=1;ji<LRU.size();ji++)
                        {
                            LRU1.push_back(LRU[ji]);
                        }
                        LRU=LRU1;
                        vector<Process*>v1;
                        for(int i5=0;i5<mainM.size();i5++)
                        {
                            if(mainM[i5]->pid!=pid)
                            {
                                v1.push_back(mainM[i5]);
                            }
                        }
                        mainM=v1;
                   }
                    LRU.push_back(process->pid);
                    curr_mm_size-=process->size;
                    process->is_in_MM=true;
                    int VPN1=0;
                    for(int ii3=0;ii3<is_empty.size();ii3++)
                    {
                        if(process->PFNs.size()==siz/PAGE_SIZE) break;
                        if(is_empty[ii3]==false)
                        {
                            is_empty[ii3]=true;
                            process->PFNs.push_back(ii3);
                            process->PT[VPN1]=ii3;
                            VPN1++;
                            process->PFNs_ocp.push_back(false);
                        }
                    }
                }
            }
                vector<int>LRU1;
                for(int j1=0;j1<LRU.size();j1++)
                {
                    if(process->pid!=LRU[j1])
                    {
                        LRU1.push_back(LRU[j1]);
                    }
                }
                LRU1.push_back(process->pid);
                LRU=LRU1;
                //cout<<"HI"<<endl;
                //cout<<process->commands.size()<<endl;
                for(int i2=0;i2<process->commands.size();i2++)
                {
                    list_commands list = process->commands[i2];
                    string name = list.name;
                    //cout<<name<<endl;
                    if(name == "load")
                    {
                        int val = list.arg1;
                        int va = list.arg2;
                        //cout<<va<<endl;
                        if(va >= process->size)
                        {
                            outputFile<<"Invalid Memory Address "<<va<<" specified for process id "<<process->pid<<endl;
                            break;
                        } 
                        int VPN = va / PAGE_SIZE;
                        //cout<<VPN<<endl;
                        int offset = va - (VPN*PAGE_SIZE);
                       // cout<<offset<<endl;
                        int PFN = process->PT[VPN];
                        // for(int i3=0;i3<process->PFNs.size();i3++)
                        // {
                        //     if(process->PFNs_ocp[i3]==false)
                        //     {
                        //         PFN=process->PFNs[i3];
                        //         process->PFNs_ocp[i3]=true;
                        //         break;
                        //     }
                        // }
                        // process->PT[VPN]=PFN;
                        //cout<<process.PT[VPN]<<" "<<PFN<<endl;
                        //cout<<process.PT[3]<<endl;
                        int PA = (PFN*PAGE_SIZE)+offset;
                        //cout<<PA<<" "<<val<<endl;
                        values[PA]=val;
                        values_v[va]=val;
                        process->VA.push_back(va);
                        process->PA.push_back(PA);
                        process->map1[va]=PA;
                        outputFile<<"Command: load "<<val<<","<<va<<";Result: Value of "<<val<<" is now stored in addr "<<PA<<endl;
                    }
                    else if(name == "add")
                    {
                        int vadr1= list.arg1;
                        if(vadr1 >= process->size)
                        {
                            outputFile<<"Invalid Memory Address "<<vadr1<<" specified for process id "<<process->pid<<endl;
                            break;
                        } 
                        int vadr2= list.arg2;
                        if(vadr2 >= process->size)
                        {
                            outputFile<<"Invalid Memory Address "<<vadr2<<" specified for process id "<<process->pid<<endl;
                            break;
                        } 
                        int vadr3 = list.arg3;
                        if(vadr3 >= process->size)
                        {
                            outputFile<<"Invalid Memory Address "<<vadr3<<" specified for process id "<<process->pid<<endl;
                            break;
                        } 
                        int VPN1 = vadr1/PAGE_SIZE ;
                        int VPN2 = vadr2/PAGE_SIZE ;
                        int VPN3 = vadr3/PAGE_SIZE ;
                        int offset1 = vadr1 - (VPN1*PAGE_SIZE);
                        int offset2 = vadr2 - (VPN2*PAGE_SIZE);
                        int offset3 = vadr3 - (VPN3*PAGE_SIZE);
                        int PFN1 = process->PT[VPN1];
                        int PFN2 = process->PT[VPN2];
                        int PFN3 = process->PT[VPN3];
                        process->PT[VPN3]=PFN3;
                        int PA1 = (PFN1*PAGE_SIZE)+offset1;
                        int PA2 = (PFN2*PAGE_SIZE)+offset2;
                        int PA3 = (PFN3*PAGE_SIZE)+offset3;
                        int x = values[PA1];
                        int y = values[PA2];
                        int z = x+y;
                        values[PA3] = z;
                        process->PA.push_back(PA3);
                        outputFile<<"Commmand: add "<<vadr1<<","<<vadr2<<","<<vadr3<<","<<";"<<"Result: Value in addr "<<vadr1<<"="<<x<<", addr "<<vadr2<<"="<<y<<",addr "<<vadr3<<"="<<z<<endl;
                    }
                    else if(name == "sub")
                    {
                        int vadr1= list.arg1;
                        if(vadr1 >= process->size)
                        {
                            outputFile<<"Invalid Memory Address "<<vadr1<<" specified for process id "<<process->pid<<endl;
                            break;
                        } 
                        int vadr2= list.arg2;
                        if(vadr2 >= process->size)
                        {
                            outputFile<<"Invalid Memory Address "<<vadr2<<" specified for process id "<<process->pid<<endl;
                            break;
                        } 
                        int vadr3 = list.arg3;
                        if(vadr3 >= process->size)
                        {
                            outputFile<<"Invalid Memory Address "<<vadr3<<" specified for process id "<<process->pid<<endl;
                            break;
                        } 
                        int VPN1 = vadr1/PAGE_SIZE ;
                        int VPN2 = vadr2/PAGE_SIZE ;
                        int VPN3 = vadr3/PAGE_SIZE ;
                        int offset1 = vadr1 - (VPN1*PAGE_SIZE);
                        int offset2 = vadr2 - (VPN2*PAGE_SIZE);
                        int offset3 = vadr3 - (VPN3*PAGE_SIZE);
                        int PFN1 = process->PT[VPN1];
                        int PFN2 = process->PT[VPN2];
                        int PFN3 = process->PT[VPN3];
                        process->PT[VPN3]=PFN3;
                        int PA1 = (PFN1*PAGE_SIZE)+offset1;
                        int PA2 = (PFN2*PAGE_SIZE)+offset2;
                        int PA3 = (PFN3*PAGE_SIZE)+offset3;
                        int x = values[PA1];
                        int y = values[PA2];
                        int z = x-y;
                        values[PA3] = z;
                        process->PA.push_back(PA3);
                        outputFile<<"Commmand: sub "<<vadr1<<","<<vadr2<<","<<vadr3<<","<<";"<<"Result: Value in addr "<<vadr1<<"="<<x<<", addr "<<vadr2<<"="<<y<<",addr "<<vadr3<<"="<<z<<endl;
                    }
                    else if(name == "print")
                    {
                        int vadr1= list.arg1;
                        if(vadr1 >= process->size)
                        {
                            outputFile<<"Invalid Memory Address "<<vadr1<<" specified for process id "<<process->pid<<endl;
                            break;
                        } 
                        int VPN1 = vadr1/PAGE_SIZE ;
                        int offset1 = vadr1 - (VPN1*PAGE_SIZE);
                        int PFN1 = process->PT[VPN1];
                        int PA1 = (PFN1*PAGE_SIZE)+offset1;
                        int x = values[PA1];
                        outputFile<<"Command: print "<<vadr1<<"; Result: Value in addr x ="<<x<<endl;
                    }
                }   
        }
        ///////////////////////////
        else if(words[0]=="kill")
        {
            string num = words[1];
            int proc_id= stoi(num);
            Process* process= proc[proc_id];
            if(proc.find(proc_id)==proc.end())
            {
                outputFile<<"Can't be killed, the pid doesn't exist"<<endl;
                continue;
            }
            if(process->is_in_MM==true)
            {
                curr_mm_size+=process->size;
                process->size = 0;
                process->PT.clear();
                for(int ik=0;ik<process->PFNs.size();ik++)
                {
                    is_empty[process->PFNs[ik]]=false;
                }
                process->PFNs.clear();
                process->PFNs_ocp.clear();
                process->commands.clear();
                for(int ik=0;ik<process->PA.size();ik++)
                {
                    values[process->PA[ik]]=0;
                }
                vector<Process*>p1;
                for(int io=0;io<mainM.size();io++)
                {
                    if(mainM[io]->pid!=process->pid)
                    {
                        p1.push_back(mainM[io]);
                    }
                }
                mainM=p1;
                process->PA.clear();
            }
            else
            {
                curr_vm_size+=process->size;
                vector<Process*>p1;
                for(int io=0;io<virtualM.size();io++)
                {
                    if(virtualM[io]->pid!=process->pid)
                    {
                        p1.push_back(virtualM[io]);
                    }
                }
                virtualM=p1;
            }
            //cout<<proc.size()<<".."<<endl;
            proc.erase(proc_id);
            //cout<<proc.size()<<"##"<<endl;
            outputFile<<"Kill Successful"<<endl;
        }
        ///////////////////////////////////////
        else if(words[0]=="listpr")
        {
            vector<int>id_M;
            for(int i7=0;i7<mainM.size();i7++)
            {
                id_M.push_back(mainM[i7]->pid);
            }
            vector<int>id_V;
            for(int i7=0;i7<virtualM.size();i7++)
            {
                id_V.push_back(virtualM[i7]->pid);
            }
            sort(id_M.begin(),id_M.end());
            sort(id_V.begin(),id_V.end());
             outputFile<<"Main memory_id : ";
            for(int i7=0;i7<id_M.size();i7++)
            {
                outputFile<<id_M[i7]<<" ";
            }
            outputFile<<"virtual id : ";
            for(int i7=0;i7<id_V.size();i7++)
            {
                outputFile<<id_V[i7]<<" ";
            }
            outputFile<<endl;
        }
        ////////////////////////////
        else if(words[0]=="pte")
        {
            string outfile1 = words[2] + ".txt";
            ofstream outputFile1(outfile1,ios::app);
            time_t now = time(0);
            char *dt = ctime(&now);
            outputFile1 << "Date and time: " << dt;
            string num = words[1];

            int proc_id= stoi(num);
            if(proc.find(proc_id)==proc.end())
            {
                outputFile1<<"Can't get pte, the pid doesn't exist"<<endl;
                continue;
            }
            Process *process= proc[proc_id]; 
            map<int,int>f = process->PT;
            for( auto it = f.begin();it!=f.end();++it)
            {
                outputFile1<<"VPN : "<<it -> first<<"  "<<"PFN : "<<it->second<<endl;
            }
        }
        else if(words[0]=="pteall")
        {
            string outfile1 = words[1] + ".txt";
            ofstream outputFile2(outfile1,ios::app);
             time_t now = time(0);
            char *dt = ctime(&now);
            outputFile2 << "Date and time: " << dt ;
            for(auto it : proc)
            {
                for( auto it1 : it.second->PT)
                {
                     outputFile2<<"VPN : "<<it1.first<<"  "<<"PFN : "<<it1.second<<endl;
                }
            }
           // cout<<"HI"<<" "<<proc.size()<<endl;
        }
        else if (words[0]=="swapout")
        {
            string num = words[1];
            int proc_id= stoi(num);
            Process *process= proc[proc_id];
            process->is_in_MM = false;
            if(proc.find(proc_id)==proc.end())
            {
                outputFile<<"Can't be swappedout, the pid doesn't exist"<<endl;
                continue;
            }
            virtualM.push_back(process);
            curr_mm_size+=process->size;
            if(curr_vm_size-process->size < 0)
            {
                outputFile<<"ERROR ,virtual memory is full"<<endl;
                continue;
            }
            curr_vm_size-=process->size;
            process->PT.clear();
            for(int ik=0;ik<process->PFNs.size();ik++)
            {
                is_empty[process->PFNs[ik]]=false;
            }
            process->PFNs.clear();
            process->PFNs_ocp.clear();
            for(int ik=0;ik<process->PA.size();ik++)
            {
                values[process->PA[ik]]=0;
            }
            vector<Process*>p1;
            for(int io=0;io<mainM.size();io++)
            {
                if(mainM[io]->pid!=process->pid)
                {
                    p1.push_back(mainM[io]);
                }
            }
            mainM=p1;
            process->PA.clear();
            outputFile<<"swapout successful"<<endl;
        }
        else if (words[0]=="swapin")
        {
           string num = words[1];
            int proc_id= stoi(num);
            //cout<<proc_id<<endl;
            Process * process = proc[proc_id];
            //Process process= proc[proc_id];
            if(!process->is_in_MM)
            {
                //cout<<"Hello"<<endl;
                vector<Process*>v2;
                for(int ii2=0;ii2<virtualM.size();ii2++)
                {
                    if(virtualM[ii2]->pid!=proc_id)
                    {
                        v2.push_back(virtualM[ii2]);
                    }
                }
                virtualM=v2;
                curr_vm_size+=process->size;
                int siz = process->size;
                if(curr_mm_size>=siz)
                {
                    mainM.push_back(process);
                    process->is_in_MM = true;
                    curr_mm_size-=siz;
                    int VPN1=0;
                    for(int ii=0;ii<is_empty.size();ii++)
                    {
                        if(process->PFNs.size()==siz/PAGE_SIZE) break;
                        if(is_empty[ii]==false)
                        {
                            is_empty[ii]=true;
                            process->PFNs.push_back(ii);
                            process->PT[VPN1]=ii;
                            VPN1++;
                            process->PFNs_ocp.push_back(false);
                        }
                    }
                }
                else
                {
                   while(siz>0)
                   {
                        int pid = LRU[0];
                        Process* old_p = proc[pid];
                        curr_mm_size+=old_p->size;
                        virtualM.push_back(old_p);
                        for(int i8=0;i8<old_p->PA.size();i8++)
                        {
                            values[old_p->PA[i8]]=0;
                        }
                        old_p->PA.clear();
                        old_p->is_in_MM = false;
                        for(int ik=0;ik<old_p->PFNs.size();ik++)
                        {
                            is_empty[old_p->PFNs[ik]]=false;
                        }
                        old_p->PFNs.clear();
                        old_p->PFNs_ocp.clear();
                        old_p->PT.clear();
                        siz-=old_p->size;
                        vector<int>LRU1;
                        for(int ji=1;ji<LRU.size();ji++)
                        {
                            LRU1.push_back(LRU[ji]);
                        }
                        LRU=LRU1;
                        vector<Process*>v1;
                        for(int i5=0;i5<mainM.size();i5++)
                        {
                            if(mainM[i5]->pid!=pid)
                            {
                                v1.push_back(mainM[i5]);
                            }
                        }
                        mainM=v1;
                   }
                    LRU.push_back(process->pid);
                    curr_mm_size-=process->size;
                    process->is_in_MM=true;
                    // for(int ik1=0;ik1<is_empty.size();ik1++)
                    // {
                    //     if(is_empty[ik1]==false)
                    //     {
                    //         is_empty[ik1]=true;
                    //         process->PFNs.push_back(ik1);
                    //         process->PFNs_ocp.push_back(false);
                    //     }
                    // }
                    int VPN1=0;
                    for(int ii1=0;ii1<is_empty.size();ii1++)
                    {
                        if(process->PFNs.size()==siz/PAGE_SIZE) break;
                        if(is_empty[ii1]==false)
                        {
                            is_empty[ii1]=true;
                            process->PFNs.push_back(ii1);
                            process->PT[VPN1]=ii1;
                            VPN1++;
                            process->PFNs_ocp.push_back(false);
                        }
                    }
                }
            }
            // for(auto it = process->map.begin();it!=process->map.end();++it)
            // {
            //     int PA = it->second;
            //     int VA = it->first;
            //     values[PA] = values_v[VA];
            // }
            for(int i=0;i<process->VA.size();i++)
            {
                int va = process->VA[i];
                int val = values_v[va];
                int VPN = (va/PAGE_SIZE);
                int offset = va - (VPN*PAGE_SIZE);
                int PFN = process->PT[VPN];
                //cout<<PFN<<endl;
                int pa = (PFN*PAGE_SIZE) + offset;
                // cout<<pa<<endl;
                values[pa]=val;
            }
            outputFile<<"swapin successful"<<endl;
        }
        else if(words[0]=="print")
        {
            string num1 = words[1];
            string num2 = words[2];
            int memloc = stoi(num1);
            int length = stoi(num2);
            for(int i13=memloc;i13<=memloc+length-1;i13++)
            {
                outputFile<<"Value of "<<i13<<": "<<values[i13]<<endl;
            }
        }
        else if(words[0]=="exit")
        {
            outputFile<<"Exited"<<endl;
            return 0;
        }
    }

 }




