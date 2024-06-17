#include <iostream>
#include "mpi.h"
#include <vector>
#include <algorithm>    //para usar o sort
#include <iomanip>  //para usar o setw
#include <cstdlib>  //para usar o system
#include <cstring> // Para usar memcpy

#define TAGVOO 5000
#define TAGCONF 4000

using namespace std;

struct Voo
{
    char codigo[10]; 
    int origem;
    int destino;   // rank destino
    int horario;   // horario marcado de saída
    int tempo_voo; // tempo que a mensagem ficará no ar
    int chegada;   // horário marcado de chegada desse voo
};

void printTabela(const vector<Voo> Pousos, const vector<Voo> Decolagens,int rank)
{
    cout << endl << "Aeroporto " << rank+1 << endl << endl;

    cout << "Pousos: " << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    cout << "| Código  | Origem  | Horário de Chegada | Tempo de Voo |" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    for (const Voo &pouso : Pousos)
    {
        cout << "| " << setw(7) << pouso.codigo << " | " << setw(7) << pouso.origem + 1 << " | "
             << setw(16) << pouso.chegada << " | " << setw(13) << pouso.tempo_voo << " |" << endl;
    }
    cout << "+---------+---------+------------------+---------------+" << endl;

    cout << "Decolagens: " << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    cout << "| Código | Destino | Horário de Partida | Tempo de Voo |" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    for (const Voo &decolagem : Decolagens)
    {
        cout << "| " << setw(7) << decolagem.codigo << " | " << setw(7) << decolagem.destino + 1 << " | "
             << setw(16) << decolagem.horario << " | " << setw(13) << decolagem.tempo_voo << " |" << endl;
    }
    cout << "+---------+---------+------------------+---------------+" << endl
         << endl
         << endl
         << endl;
}

vector<Voo> getVoos(int *log)
{
    vector<Voo> decolagens;
    int quantidade;
    cout << "Digite a quantidade de decolagens (-1 para decolagens predefinidas (feito para 3 processos)): ";
    cin >> quantidade;
    cout << "Deseja mostrar os LOGS que ocorrem ao decorrer do programa? -1 = sim, 0 = não" << endl;
    cin >> *log;

    if (quantidade < 0){        //entrada para testes, escolha o return e coloque como primeiro return para testar
        return {    //entrada para 3 processos
            // codigo, origem, destino, horario, tempo_voo, horario marcado de chegada
            {"AA101", 0, 1, 0, 2, 2},
            {"EE505", 0, 1, 4, 5, 9},
            {"BB202", 0, 1, 1, 3, 4},
            {"DD404", 0, 2, 3, 4, 7},
            {"CC303", 0, 2, 2, 3, 5},

            {"JJ010", 1, 0, 4, 5, 9},
            {"FF606", 1, 0, 0, 2, 2},
            {"HH808", 1, 2, 2, 3, 5},
            {"GG707", 1, 2, 1, 3, 4},
            {"II909", 1, 0, 3, 4, 7},
            
            {"KK111", 2, 0, 0, 3, 3},
            {"MM333", 2, 1, 2, 4, 6},
            {"LL222", 2, 1, 1, 2, 3},
            {"NN444", 2, 1, 3, 5, 8}};

        //outros exemplos
        return {    //entrada para 6 processos
            // codigo, origem, destino, horario, tempo_voo, horario marcado de chegada
            {"AA101", 0, 1, 0, 2, 2},
            {"AB102", 0, 2, 1, 3, 4},
            {"AC103", 0, 3, 2, 3, 5},
            {"AD104", 0, 4, 3, 2, 5},
            {"AE105", 0, 5, 4, 1, 5},

            {"BA201", 1, 0, 1, 2, 3},
            {"BB202", 1, 2, 2, 2, 4},
            {"BC203", 1, 3, 3, 3, 6},
            {"BD204", 1, 4, 4, 1, 5},
            {"BE205", 1, 5, 0, 5, 5},

            {"CA301", 2, 0, 2, 1, 3},
            {"CB302", 2, 1, 0, 3, 3},
            {"CC303", 2, 3, 3, 2, 5},
            {"CD304", 2, 4, 4, 3, 7},
            {"CE305", 2, 5, 5, 1, 6},

            {"DA401", 3, 0, 3, 2, 5},
            {"DB402", 3, 1, 2, 1, 3},
            {"DC403", 3, 2, 3, 2, 5},
            {"DD404", 3, 4, 4, 3, 7},
            {"DE405", 3, 5, 5, 1, 6},

            {"EA501", 4, 0, 4, 1, 5},
            {"EB502", 4, 1, 3, 1, 4},
            {"EC503", 4, 2, 4, 2, 6},
            {"ED504", 4, 3, 0, 3, 3},
            {"EE505", 4, 5, 2, 2, 7},

            {"FA601", 5, 0, 5, 1, 6},
            {"FB602", 5, 1, 4, 1, 5},
            {"FC603", 5, 2, 5, 2, 7},
            {"FD604", 5, 3, 1, 2, 3},
            {"FE605", 5, 4, 3, 1, 4}
        };

        return {//entrada para 4 processos
            // codigo, origem, destino, horario, tempo_voo, horario marcado de chegada
            {"AA101", 0, 1, 0, 2, 2},
            {"EE505", 0, 1, 4, 5, 9},
            {"BB202", 0, 1, 1, 3, 4},
            {"DD404", 0, 2, 3, 4, 7},
            {"CC303", 0, 2, 2, 3, 5},

            {"JJ010", 1, 0, 4, 5, 9},
            {"FF606", 1, 0, 0, 2, 2},
            {"HH808", 1, 2, 2, 3, 5},
            {"GG707", 1, 2, 1, 3, 4},
            {"II909", 1, 0, 3, 4, 7},

            {"KK111", 2, 0, 0, 3, 3},
            {"MM333", 2, 1, 2, 4, 6},
            {"LL222", 2, 1, 1, 2, 3},
            {"NN444", 2, 1, 3, 5, 8},

            {"PP555", 3, 0, 3, 4, 7},
            {"QQ666", 3, 1, 0, 2, 2},
            {"RR777", 3, 1, 2, 4, 6},
            {"SS888", 3, 2, 1, 3, 4},
            {"TT999", 3, 2, 2, 3, 5}};

        return {//entrada simples para 3 processos
            // codigo, origem, destino, horario, tempo_voo, horario marcado de chegada
            {"AA101", 0, 1, 0, 2, 2},
            {"BB202", 0, 1, 1, 3, 4},  
            {"CC303", 0, 2, 2, 3, 5},

            {"DD404", 1, 2, 1, 4, 5},
            {"EE505", 1, 0, 2, 6, 8},

            {"FF606", 2, 0, 5, 1, 6},
            {"GG707", 2, 1, 7, 1, 8}};
    }

    for (int i = 0; i < quantidade; ++i)
    {
        cout << "Voo ( " << i + 1 << " )" << endl;
        Voo voo;
        cout << "Digite origem do voo: (aeroporto >= 1) ";
        cin >> voo.origem;
        voo.origem--;
        cout << "Digite o destino do voo: (aeroporto >= 1) ";
        cin >> voo.destino;
        voo.destino--;
        cout << "Digite o horário do voo: ";
        cin >> voo.horario;
        cout << "Digite o tempo do voo: ";
        cin >> voo.tempo_voo;
        voo.chegada = voo.horario + voo.tempo_voo;

        decolagens.push_back(voo);
    }

    vector<int> codigos(quantidade,1);

    for(Voo &voo : decolagens){
        voo.chegada = voo.horario + voo.tempo_voo;
        snprintf(voo.codigo, sizeof(voo.codigo), "%d-%d", voo.origem + 1, codigos[voo.origem]++);
    }

    sort(decolagens.begin(), decolagens.end(), [](const Voo& a, const Voo& b) {
        return a.origem < b.origem;
    });

    return decolagens;
}

vector<vector<int>> getAterrissagem(const vector<Voo> Dados, int size)
{
    vector<vector<int>> aterrisagens(size);

    for (const Voo &voo : Dados)
    {
        aterrisagens[voo.destino].push_back(voo.chegada);
        
        sort(aterrisagens[voo.destino].begin(), aterrisagens[voo.destino].end());
    }

    return aterrisagens;
}

void printAterrissagem(const vector<int> aterrissagem,int rank)
{
    cout << "Horários de chegada na máquina " << rank << ": ";
    for (int i = 0; i < aterrissagem.size(); i++)
    {
        cout << aterrissagem[i] << " ";
    }
    cout << endl;
}

Voo Get_Melhor_voo(vector<Voo> Voos, int *pos) {
    Voo milior;
    if (!Voos.empty()) {
        milior = Voos[0];
        *pos = 0;
        for (int i = 1; i < Voos.size(); ++i) {
            if (milior.tempo_voo < Voos[i].tempo_voo) {
                milior = Voos[i];
                *pos = i;
            }
        }
    }
    return milior;
}

int getMaxIda(vector<Voo> Voos){
    int max=0;
    if (!Voos.empty()) {
        max = Voos[0].chegada;

        for (Voo voo : Voos) {
            if (max < voo.chegada) {
                max = voo.chegada;
            }
        }
    }
    return max;
}

void acrescentar_ida(vector<Voo> &voos, int indice = 0)
{

    if (indice >= voos.size() - 1)
    {
        return;
    }

    if (voos[indice].horario == voos[indice + 1].horario)
    {
        voos[indice + 1].horario++;
    }

    if (indice + 2 < voos.size() && voos[indice + 1].horario == voos[indice + 2].horario)
    {
        acrescentar_ida(voos, indice + 1);
    }

    acrescentar_ida(voos, indice + 1);
}

int get_cont(vector<int> vetor, int valor)
{
    int cont = 0;
    for(int numero : vetor)
    {
        if(numero == valor)
            cont ++;
        
    }
    return cont;
}

void receber_confirmacoes(vector<Voo> &decolagens, int clock){
    MPI_Request r;

    for(Voo &voo : decolagens) 
        if (voo.chegada == clock)   //recebendo confirmação se pode chegar nesse horario
        {
            int confirmacao;
            cout << "esperando confirmação ..." << endl;
            MPI_Recv(&confirmacao, 1, MPI_INT, voo.destino, TAGCONF, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (confirmacao)    //voo entregue com sucesso
                cout << "Voo chegou no outro aeroporto com sucesso" << endl;
            else{    //voo atrasará mais um tempo lógico
                voo.chegada++;
                voo.tempo_voo++;
                cout << "Voo " << voo.codigo << " não recebido, proxima tentativa: " << voo.chegada << endl;
            }
        }
}

void envios(vector<Voo> &decolagens, int clock){
    for(Voo voo : decolagens)   //mais de um voo para chegar nesse horario
        if (voo.chegada == clock){   //enviando nesse horario de chegada
            cout<<"Decolando voo..."<<endl;
            MPI_Send(&voo, sizeof(Voo),MPI_BYTE, voo.destino, TAGVOO, MPI_COMM_WORLD);
        }
}

void trafego(vector<Voo> &pousos, vector<Voo> &decolagens, vector<int> aterrisagem, int rank){
    vector<Voo> voosanalisados;
    Voo voo_aterrisando;
    //vector<int> atrasoida(decolagens.size(),0);
    MPI_Request request_chegada;
    MPI_Request request_conf_true;
    MPI_Request request_conf_false;
    int stop = 1;
    int timelimit = 50;     //controlar tempo máximo desejado
    int global_var = 1;

    for (int clock = 0; global_var && clock < timelimit; clock++)
    {
        cout << endl << "Tempo lógico "<< clock << endl;
        
        int qntChegada = get_cont(aterrisagem, clock);
        
        cout << "Nesse loop chegou " << qntChegada << endl;
        printAterrissagem(aterrisagem,rank);
        
        voosanalisados.clear();

        // Recebimento de voos
        for (int i = 0; i < qntChegada; i++)
        {
            cout << "Recebendo gente" <<endl;
            MPI_Irecv(&voo_aterrisando, sizeof(Voo), MPI_BYTE, MPI_ANY_SOURCE, TAGVOO, MPI_COMM_WORLD, &request_chegada);

            if(!i)   //so fazer os envios uma vez
                envios(decolagens, clock);

            MPI_Wait(&request_chegada, MPI_STATUS_IGNORE);

            voosanalisados.push_back(voo_aterrisando);
        }

        if(qntChegada > 0){
            Voo bestVoo;
            if(!voosanalisados.empty()){
                int pos;    //posição a ser removida de vooanalisados para enviar false para outros processos
                bestVoo = Get_Melhor_voo(voosanalisados, &pos);

                pousos.push_back(bestVoo);
                cout << "Voo pousado vindo de " << bestVoo.origem <<", Voo: "<< bestVoo.codigo << endl;
                
                /*
                cout << "voos analisados origens antes: ";
                for (Voo voo : voosanalisados)  
                    cout << voo.origem << " ";
                cout<<endl;
                */ 

                int verdadeiro=1;
                MPI_Isend(&verdadeiro, 1, MPI_INT, bestVoo.origem, TAGCONF, MPI_COMM_WORLD, &request_conf_true);

                voosanalisados.erase(voosanalisados.begin() + pos); //mensagem de confirmação já enviada
                aterrisagem.erase(aterrisagem.begin()); //chegou com sucesso, dropa sua chegada

                /*
                cout << "voos analisados origens depois: ";
                for (Voo voo : voosanalisados)  
                    cout << voo.origem << " ";
                cout<<endl;
                */
            }

            for (Voo voo : voosanalisados)  //Respondendo as outras máquinas
            {
                int falso = 0;
                cout << "enviando false para maquina " << voo.origem <<endl;
                MPI_Isend(&falso, 1, MPI_INT, voo.origem, TAGCONF, MPI_COMM_WORLD, &request_conf_false);
                aterrisagem[0]++; 
                sort(aterrisagem.begin(),aterrisagem.end());

                //MPI_Wait(&request_conf_false, MPI_STATUS_IGNORE);
            }

            receber_confirmacoes(decolagens, clock);

            //MPI_Wait(&request_conf_true, MPI_STATUS_IGNORE);

            /*
            // correções se existe voo para partir agora
            int i = 0;
            for (Voo voo : decolagens)
            {
                if (voo.horario == clock)   //alguem irá partir nesse horário?
                {
                    decolagens[i].horario++;
                    //acrescentar_ida(decolagens, i);
                    decolagens[i].chegada++;    //preciso avisar que vai chegar com atraso (atualizar chegada no outro processo)
                    break;
                }
                i++;
            }
            */
        }
        else    //se qntChegada <= 0 envios ainda não foram feitos nesse clock
        {
            envios(decolagens, clock);
            receber_confirmacoes(decolagens, clock);
        }
    
        //Para cada tempo lógico sincroniza os processos
        MPI_Barrier(MPI_COMM_WORLD);   

        //verifica continuidade no próximo loop
        if(aterrisagem.empty() && clock > getMaxIda(decolagens))
            stop = 0;
        
        MPI_Allreduce(&stop, &global_var, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<Voo> pousos;         //resultado que queremos receber
    vector<Voo> decolagens;     //decolagens vindas do usuário sem correções de conflitos
    vector<int> aterrisagem;    //vetor contendo tempo lógico em que virão mensagem de outros processos
    int log;                    //caso queira mostrar no console informações de cada tempo lógico em cada maquina

    //recebimento e compartilhamento dos dados entre os aeroportos
    if (rank == 0)  
    {
        vector<Voo> Dados = getVoos(&log);
        vector<vector<Voo>> Blocos_voo(size);

        for (const Voo &voo : Dados)
        {
            Blocos_voo[voo.origem].push_back(voo);
        }

        vector<vector<int>> Matriz_aterrissagem = getAterrissagem(Dados, size);

        for (int i = 1; i < size; i++)
        {
            sort(Blocos_voo[i].begin(), Blocos_voo[i].end(), [](const Voo& a, const Voo& b) {
                return a.chegada < b.chegada;
            });

            for (const Voo &voo : Blocos_voo[i])
            {
                MPI_Send(&voo, sizeof(Voo), MPI_BYTE, i, TAGVOO, MPI_COMM_WORLD);
            }
            Voo voo_paia = {"-1", -1, -1, -1, -1, -1};
            MPI_Send(&voo_paia, sizeof(Voo), MPI_BYTE, i, TAGVOO, MPI_COMM_WORLD);

            int tam = Matriz_aterrissagem[i].size();
            
            MPI_Send(&tam, 1, MPI_INT, i, TAGVOO, MPI_COMM_WORLD);
            MPI_Send(Matriz_aterrissagem[i].data(), tam, MPI_INT, i, TAGVOO, MPI_COMM_WORLD);
        }

        sort(Blocos_voo[0].begin(), Blocos_voo[0].end(), [](const Voo& a, const Voo& b) {
            return a.chegada < b.chegada;
        });
        
        decolagens = Blocos_voo[0];
        aterrisagem = Matriz_aterrissagem[0];
    }
    else
    {
        while (true)
        {
            Voo voo;
            MPI_Recv(&voo, sizeof(Voo), MPI_BYTE, 0, TAGVOO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (voo.origem != -1)
            {
                decolagens.push_back(voo);
            }
            else
            {
                int tam;
                MPI_Recv(&tam, 1, MPI_INT, 0, TAGVOO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  // Receber tamanho

                aterrisagem.resize(tam);

                MPI_Recv(aterrisagem.data(), tam, MPI_INT, 0, TAGVOO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  // Receber dados
                break;
            }
        }
    }

    cout << "Aeroporto " << rank + 1 << endl;

    //lógica do gerenciamento de todos os aeroportos
    trafego(pousos, decolagens, aterrisagem, rank);

    //limpando LOGS do terminal
    MPI_Bcast(&log, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(!log)
        system("clear");

    /*resposta*/
    printTabela(pousos, decolagens, rank);
    MPI_Finalize();

    return 0;
}
