#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include "mpi/mpi.h"
#include<fstream>
#include"time.h"
#include <math.h>
using namespace std;

const int route=0;
const int circle[] = {0,2,7,6,3,5,4,1};
const int procNumber=8;
const int N1=200;
const int N2=216;
const int N3=117;
int procRank;
const int root=4;
MPI_Status Status;
struct A{
    int row;
    int n1;
    int n2;
    double arr[N1/8+N1][N2];
};
struct B{
    int col;
    int n2;
    int n3;
    double arr[N2][N3/8+N3];
};
struct R{
    int n1;
    int n3;
    double arr[N1/8+N1][N3];
};
double matrixA[N1][N2]={0};
double matrixB[N2][N3]={0};
double Res[N1][N3]={0};
A subA[procNumber];
B subB[procNumber];
A procA;
B procB;
B procBtmp;
R procRes;
R result[procNumber];
int NextProc;
int PrevProc;
int procSize;
//Збір
std::ofstream MyFile_log;
void DataReplication(){
    MyFile_log.open("/home/lemko/Documents/log_mpi_kursak.txt",ios::app);
    MyFile_log<<"Початок збирання кінцевих даних між процесорами  \n";
    if(procRank==7||procRank==5||procRank==0){//
        MPI_Send(&procRes,sizeof(procRes),MPI_BYTE,root,0,MPI_COMM_WORLD);
        if(procRank==7){
            MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № 7."<<" Надсилає до процесора № "<<root<<"\n";
            MPI_Recv(&procRes,sizeof(procRes),MPI_BYTE,6,0,MPI_COMM_WORLD,&Status);//5
            MPI_Send(&procRes,sizeof(procRes),MPI_BYTE,root,0,MPI_COMM_WORLD);
        }
        if(procRank==5){
            MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № 5."<<" Надсилає до процесора № "<<root<<"\n";
            MPI_Recv(&procRes,sizeof(procRes),MPI_BYTE,3,0,MPI_COMM_WORLD,&Status);//2
            MPI_Send(&procRes,sizeof(procRes),MPI_BYTE,root,0,MPI_COMM_WORLD);
        }
        if(procRank==0){
            MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № 0."<<" Надсилає до процесора № "<<root<<"\n";
            MPI_Recv(&procRes,sizeof(procRes),MPI_BYTE,1,0,MPI_COMM_WORLD,&Status);//1
            MPI_Send(&procRes,sizeof(procRes),MPI_BYTE,root,0,MPI_COMM_WORLD);
        }
    }
    if(procRank==3){
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<5<<"\n";
        MPI_Send(&procRes,sizeof(procRes),MPI_BYTE,5,0,MPI_COMM_WORLD);//7
    }
    if(procRank==2){
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<root<<"\n";
        MPI_Send(&procRes,sizeof(procRes),MPI_BYTE,root,0,MPI_COMM_WORLD);//3
    }

    if(procRank==1)
    {
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<0<<"\n";
        MPI_Send(&procRes,sizeof(procRes),MPI_BYTE,0,0,MPI_COMM_WORLD);//1
    }
    if(procRank==6){
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<7<<"\n";
        MPI_Send(&procRes,sizeof(procRes),MPI_BYTE,7,0,MPI_COMM_WORLD);//4
    }
    if(procRank==root){
        result[root]=procRes;
        MPI_Recv(&result[6],sizeof(procRes),MPI_BYTE,7,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<7<<"\n";
        MPI_Recv(&result[3],sizeof(procRes),MPI_BYTE,5,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<5<<"\n";
        MPI_Recv(&result[1],sizeof(procRes),MPI_BYTE,0,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора №"<<0<<"\n";
        MPI_Recv(&result[7],sizeof(procRes),MPI_BYTE,7,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<7<<"\n";
        MPI_Recv(&result[5],sizeof(procRes),MPI_BYTE,5,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<5<<"\n";
        MPI_Recv(&result[0],sizeof(procRes),MPI_BYTE,0,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<0<<"\n";
        MPI_Recv(&result[2],sizeof(procRes),MPI_BYTE,2,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<2<<"\n";
    }
    MyFile_log<<"Закінчення збирання кінцевих даних між процесорами  \n";
    MyFile_log.close();
}
//початкова розсилка
void DataDistribution(){
    MPI_Status Status;
    MyFile_log.open("/home/lemko/Documents/log_mpi_kursak.txt",ios::app);
    MyFile_log<<"Початок пересилання початкових даних між процесорами  \n";
    if(procRank==root){
        procA = subA[root];
        procB = subB[root];
        MPI_Send(&subA[2],sizeof(subA[2]),MPI_BYTE,0,0,MPI_COMM_WORLD);
        MPI_Send(&subB[2],sizeof(subB[2]),MPI_BYTE,0,0,MPI_COMM_WORLD);
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<0<<"\n";
        MPI_Send(&subA[6],sizeof(subA[6]),MPI_BYTE,1,0,MPI_COMM_WORLD);
        MPI_Send(&subB[6],sizeof(subB[6]),MPI_BYTE,1,0,MPI_COMM_WORLD);
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<1<<"\n";
        MPI_Send(&subA[7],sizeof(subA[7]),MPI_BYTE,3,0,MPI_COMM_WORLD);
        MPI_Send(&subB[7],sizeof(subB[7]),MPI_BYTE,3,0,MPI_COMM_WORLD);
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<3<<"\n";
        MPI_Send(&subA[0],sizeof(subA[0]),MPI_BYTE,0,0,MPI_COMM_WORLD);
        MPI_Send(&subB[0],sizeof(subB[0]),MPI_BYTE,0,0,MPI_COMM_WORLD);
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<0<<"\n";
        MPI_Send(&subA[1],sizeof(subA[1]),MPI_BYTE,1,0,MPI_COMM_WORLD);
        MPI_Send(&subB[1],sizeof(subB[1]),MPI_BYTE,1,0,MPI_COMM_WORLD);
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<1<<"\n";
        MPI_Send(&subA[3],sizeof(subA[3]),MPI_BYTE,3,0,MPI_COMM_WORLD);
        MPI_Send(&subB[3],sizeof(subB[3]),MPI_BYTE,3,0,MPI_COMM_WORLD);
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<3<<"\n";
        MPI_Send(&subA[5],sizeof(subA[5]),MPI_BYTE,5,0,MPI_COMM_WORLD);
        MPI_Send(&subB[5],sizeof(subB[5]),MPI_BYTE,5,0,MPI_COMM_WORLD);
        MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<5<<"\n";
    }
    if(procRank==0 || procRank==3||procRank==1){
        MPI_Recv(&procA,sizeof(procA),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
        MPI_Recv(&procB,sizeof(procB),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<root<<"\n";
        if(procRank==0){
            MPI_Send(&procA,sizeof(procA),MPI_BYTE,2,0,MPI_COMM_WORLD);
            MPI_Send(&procB,sizeof(procB),MPI_BYTE,2,0,MPI_COMM_WORLD);
            MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<2<<"\n";
            MPI_Recv(&procA,sizeof(procA),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
            MPI_Recv(&procB,sizeof(procB),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
            MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<root<<"\n";
        }
        if(procRank==3){
            MPI_Send(&procA,sizeof(procA),MPI_BYTE,7,0,MPI_COMM_WORLD);
            MPI_Send(&procB,sizeof(procB),MPI_BYTE,7,0,MPI_COMM_WORLD);
            MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<7<<"\n";
            MPI_Recv(&procA,sizeof(procA),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
            MPI_Recv(&procB,sizeof(procB),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
            MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<root<<"\n";
        }
        if(procRank==1){
            MPI_Send(&procA,sizeof(procA),MPI_BYTE,6,0,MPI_COMM_WORLD);
            MPI_Send(&procB,sizeof(procB),MPI_BYTE,6,0,MPI_COMM_WORLD);
            MyFile_log<<"Процесор №: "<<procRank<<" Надсилає до процесора № "<<6<<"\n";
            MPI_Recv(&procA,sizeof(procA),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
            MPI_Recv(&procB,sizeof(procB),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
            MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<root<<"\n";;
        }
    }
    if(procRank==7){

        MPI_Recv(&procA,sizeof(procA),MPI_BYTE,3,0,MPI_COMM_WORLD,&Status);
        MPI_Recv(&procB,sizeof(procB),MPI_BYTE,3,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<3<<"\n";
    }
    if(procRank==2){
        MPI_Recv(&procA,sizeof(procA),MPI_BYTE,0,0,MPI_COMM_WORLD,&Status);
        MPI_Recv(&procB,sizeof(procB),MPI_BYTE,0,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<0<<"\n";
    }
    if(procRank==6){
        MPI_Recv(&procA,sizeof(procA),MPI_BYTE,1,0,MPI_COMM_WORLD,&Status);
        MPI_Recv(&procB,sizeof(procB),MPI_BYTE,1,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<1<<"\n";
    }
    if(procRank==5){
        MPI_Recv(&procA,sizeof(procA),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
        MPI_Recv(&procB,sizeof(procB),MPI_BYTE,root,0,MPI_COMM_WORLD,&Status);
        MyFile_log<<"Процесор №: "<<procRank<<" отримує від процесора № "<<root<<"\n";
    }
    MyFile_log<<"Закінчення пересилання початкових даних між процесорами  \n";
    MyFile_log.close();
}
//пересилка підматриці і перемноження
void mull(){
    for(int i=0;i<procA.n1;i++)
        for(int j=0;j<procB.n3;j++)
            for(int k=0;k<procA.n2;k++)
                procRes.arr[i][j+procB.col]+=procA.arr[i][k]*procB.arr[k][j];
}
void DataSend(){
    MyFile_log.open("/home/lemko/Documents/log_mpi_kursak.txt",ios::app);
    MyFile_log<<"Початок пересилання даних між процесорами по кільцевій структурі \n";
    procRes.n1=procA.n1;
    procRes.n3=N3;
    for(int k=0;k<procNumber;k++){
        for(int i=0;i<procNumber;i++){
            if(procRank==circle[i]){
                if( i % 2==0 ) {
                    MPI_Send(&procB,sizeof(B),MPI_BYTE,circle[(i+1)%procSize],0,MPI_COMM_WORLD);
                    MyFile_log<<"Процесор №: "<<i<<" Надсилає до процесора № "<<circle[(i+1)%procSize]<<"\n";
                    MPI_Recv(&procBtmp,sizeof(B),MPI_BYTE,circle[(i+procSize-1)%procSize],0,MPI_COMM_WORLD,&Status);
                    MyFile_log<<"Процесор №: "<<i<<" отримує від процесора № "<<circle[(i+procSize-1)%procSize]<<"\n";
                    procB=procBtmp;
                } else {
                    MPI_Recv(&procBtmp,sizeof(B),MPI_BYTE,circle[(i-1)%procSize],0,MPI_COMM_WORLD,&Status);
                    MyFile_log<<"Процесор №: "<<i<<" отримує від процесора № "<<circle[(i-1)%procSize]<<"\n";
                    MPI_Send(&procB,sizeof(B),MPI_BYTE,circle[(i+1)%procSize],0,MPI_COMM_WORLD);
                    MyFile_log<<"Процесор №: "<<i<<" Надсилає до процесора № "<<circle[(i+1)%procSize]<<"\n";
                    procB=procBtmp;
                }
            }
        }
        mull();
    }
    MyFile_log<<"Закінчення пересилання даних між процесорами по кільцевій структурі \n";
    MyFile_log.close();
}

void SetRank()
{
    if(procRank==0)
    {
        NextProc = 2;
        PrevProc = 1;
    }
    if(procRank==1)
    {
        NextProc = 0;
        PrevProc = 4;
    }
    if(procRank==2)
    {
        NextProc = 7;
        PrevProc = 0;
    }
    if(procRank==3)
    {
        NextProc = 5;
        PrevProc = 6;
    }
    if(procRank==4)
    {
        NextProc = 1;
        PrevProc = 5;
    }
    if(procRank==5)
    {
        NextProc = 4;
        PrevProc = 3;
    }
    if(procRank==6)
    {
        NextProc = 3;
        PrevProc = 7;
    }
    if(procRank==7)
    {
        NextProc = 6;
        PrevProc = 2;
    }
}
clock_t t;
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&procRank);
    MPI_Comm_size(MPI_COMM_WORLD,&procSize);
    if(procSize!=8){
        cout<<"Must be 8 process"<<endl;
        MPI_Finalize();
        return 1;
    }
    if(procRank==root){
        //заповнення А
        std::ifstream file_a;
        file_a.open("/home/lemko/Documents/Matrix_a.txt");
        for(int i=0;i<N1;i++)
            for(int j=0;j<N2;j++)
                file_a  >>matrixA[i][j];
        cout<<"Matrix A"<<endl;
        for(int i=0;i<N1;i++){
            for(int j=0;j<N2;j++)
                cout<<matrixA[i][j]<<" ";
            cout<<endl;
        }
        file_a.close();
        //заповнення В
        std::ifstream file_b;
        file_b.open("/home/lemko/Documents/Matrix_b.txt");
        for(int i=0;i<N2;i++)
            for(int j=0;j<N3;j++)
                file_b>>  matrixB[i][j];
        cout<<endl<<"Matrix B"<<endl;
        for(int i=0;i<N2;i++){
            for(int j=0;j<N3;j++)
                cout<<matrixB[i][j]<<" ";
            cout<<endl;
        }
        file_b.close();
        //перемноження матриць

        t=clock();
        std::ofstream Myfile_rez1;
        Myfile_rez1.open("/home/lemko/Documents/Matrix_rez_pos.txt",std::ios::in);
        for(int i=0;i<N1;i++)
            for(int j=0;j<N3;j++)
                for(int k=0;k<N2;k++)
                    Res[i][j]+=matrixA[i][k]*matrixB[k][j];
        cout<<endl<<"Result"<<endl;
        for(int i=0;i<N1;i++){
            for(int j=0;j<N3;j++){
                cout<<Res[i][j]<<" ";
                Myfile_rez1<<Res[i][j]<<" ";
            }
            cout<<endl;
            Myfile_rez1<<endl;
        }
        Myfile_rez1.close();
        t=clock()-t;
        //розбиття на підматриці
        //А
        int rowsSum=0;
        int restRows = N1;
        int restProc = procNumber;
        for(int i=0;i<procNumber;i++)
        {
            subA[i].row=rowsSum;
            subA[i].n1 = restRows/restProc;
            subA[i].n2 = N2;
            for (int k=0;k<subA[i].n1;k++)
                for(int j=0;j<subA[i].n2;j++)
                    subA[i].arr[k][j] = matrixA[k+rowsSum][j];
            rowsSum+=subA[i].n1;
            restRows -= restRows/restProc;
            restProc--;
        }
        //В
        rowsSum=0;
        restProc = procNumber;
        restRows = N3;
        for(int i=0;i<procNumber;i++)
        {
            subB[i].col = rowsSum;
            subB[i].n2 = N2;
            subB[i].n3 = restRows/restProc;
            for (int k= 0;k<subB[i].n2;k++)
                for(int j=0;j<subB[i].n3;j++)
                    subB[i].arr[k][j] = matrixB[k][j+rowsSum];
            rowsSum+=subB[i].n3;
            restRows -= restRows/restProc;
            restProc--;
        }
    }
    double beg,end,rez1;
    //задає кільцеву структуру
    SetRank();
    //початкова розсилка
    DataDistribution();
    //розсилка і перемноження
    DataSend();
    //збір даних
    DataReplication();

    beg=MPI_Wtime();
    std::ofstream Myfile_rez;
    Myfile_rez.open("/home/lemko/Documents/Matrix_rez_par.txt",std::ios::in);
    if(procRank==root){
        cout<<endl<<"Paralel rez"<<endl;
        for(int k=0;k<procNumber;k++)
            for(int i=0;i<result[k].n1;i++){
                for(int j=0;j<result[k].n3;j++){
                    cout<<result[k].arr[i][j]<<" ";
                    Myfile_rez<<result[k].arr[i][j]<<" ";
                }
                cout<<endl;
                Myfile_rez<<endl;
            }
    }
    Myfile_rez.close();
    end=MPI_Wtime();
    rez1=end-beg;
    double rezpos=(((double)t)/CLOCKS_PER_SEC);
    std::ofstream Myfile_time_par;
    Myfile_time_par.open("/home/lemko/Documents/Matrix_time.txt",std::ios::in);
    Myfile_time_par<<rez1<<"\n";
    Myfile_time_par<<rezpos<<"\n";
    Myfile_time_par.close();
    MPI_Finalize();
    return 0;
}

