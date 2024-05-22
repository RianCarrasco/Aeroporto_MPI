#include <iostream>
#include "mpi.h"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

#define D 1 // Variavel que eu são sei qual é

using namespace std;



const int TAG = 0;
const int REQ_TAG = 1;
const int ACK_TAG = 2;

struct Voo {
    int codigo;
    int origem;
    int destino;//rank destino 
    int horario;// qual tempo logico ele deve ir 
    int tempo_voo;//tempo que a mensagem ficará no ar
    bool is_pouso;
};

int Lamport(int pi)
{
    return (pi * 2) + D;
}

struct Mensagem {
    int lc; // Relógio lógico
    Voo voo;
};

void printTabela(const vector<Mensagem>& mensagens, int rank) {
    cout << "Tabela: Configuração para o aeroporto (" << rank + 1 << ")" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    cout << "| Código  | Origem  | Horário de Chegada | Tempo de Voo |" << endl;
    cout << "| Pousos  |---------|--------------------|--------------|" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    for (const auto& msg : mensagens) {
        if (msg.voo.is_pouso) {
            cout << "| " << setw(7) << msg.voo.codigo << " | " << setw(7) << msg.voo.origem << " | "
                << setw(16) << msg.voo.horario << " | " << setw(13) << msg.voo.tempo_voo << " |" << endl;
        }
    }
    cout << "+---------+---------+------------------+---------------+" << endl;
    cout << "|Decola..| Destino | Horário de Partida | Tempo de Voo |" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    for (const auto& msg : mensagens) {
        if (!msg.voo.is_pouso) {
            cout << "| " << setw(7) << msg.voo.codigo << " | " << setw(7) << msg.voo.destino << " | "
                << setw(16) << msg.voo.horario << " | " << setw(13) << msg.voo.tempo_voo << " |" << endl;
        }
    }
    cout << "+---------+---------+------------------+---------------+" << endl << endl << endl;
}

bool compareMensagens(const Mensagem& a, const Mensagem& b) {
    if (a.voo.horario == b.voo.horario) {
        return a.voo.tempo_voo > b.voo.tempo_voo;
    }
    return a.voo.horario < b.voo.horario;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    const int NUM_AEROPORTOS = size;
    int lc = 0; // Relógio lógico
    int pi = 0;
    char resposta;
    bool iniciar_tudo = false;
    vector<Voo> pousos;
    vector<Voo> decolagens = {//codigo, origem, destino, horario, tempo_voo, is_pouso
        {1, 0, 1, 0, 3, false},
        {2, 1, 0, 0, 3, false},
        {3, 1, 0, 0, 2, false}
    };
    vector<Mensagem> mensagens;

    /*
    do
    {
        cout << "Olá gerente do aeroporto, desgne os voos a serem feitos" << endl;
        cout << "Gosraria de adicionar um voo? (s/n)" << endl;
        cin >> resposta;
        if(resposta == 's' || resposta == 'S')
        {
            Voo novo_voo;
            cout << "Digite o código do voo: " << endl;
            cin >> novo_voo.codigo;
            cout << "Digite a origem do voo: " << endl;
            cin >> novo_voo.origem;
            cout << "Digite o destino do voo: " << endl;
            cin >> novo_voo.destino;
            cout << "Digite o horário de partida do voo: " << endl;
            cin >> novo_voo.horario;
            cout << "Digite o tempo de voo: " << endl;
            cin >> novo_voo.tempo_voo;
            novo_voo.is_pouso = false;
            decolagens.push_back(novo_voo);
        }
        else
        {
            cout << "Iniciando voos" << endl;
            system("clear");
            break;
        }
    }while (true);
    */
    // Formando códigos de voos 
    int pouso_count = 1;
    int decolagem_count = 1;
    string resultado;

    for (auto& voo : decolagens) {
        if(voo.origem == rank)
        {   
            lc++;
            resultado = to_string(((rank + 1) * 10) + decolagem_count);//Primeiro numero é a origem, segundo é a quantidade de decolaegens
            voo.codigo = atoi(resultado.c_str());
            decolagem_count++;

            Mensagem msg = { lc, voo };
            mensagens.push_back(msg);
            //decolagens.erase(decolagens.begin() + cont_total);//deixando apenas voos que não são do aeroporto atual
        }
    }


    // Enviar decolagens para os destinos
    

    for (auto& msg : mensagens) {
        
        MPI_Send(&msg, sizeof(Mensagem), MPI_BYTE, msg.voo.destino, TAG, MPI_COMM_WORLD);
    }

    // Receber pousos de outros aeroportos
    for (int i = 0; i < size - 1; i++) {
        MPI_Status status;
        Mensagem msg;
        MPI_Recv(&msg, sizeof(Mensagem), MPI_BYTE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
        msg.voo.is_pouso = true;
        pousos.push_back(msg.voo);
        mensagens.push_back(msg);
    }

    // Priorizar pousos e decolagens


    sort(msg.begin(), msg.end(), compareMensagens);

    // Simular o uso da pista obedecendo as prioridades
    vector<Mensagem> pista;
    int current_time = 0;

    for(auto& men: msg)
    {
        
    }

    for (const auto& msg : mensagens) {
        if (msg.voo.horario > current_time) {
            current_time = msg.voo.horario;
        } else {
            current_time++;
        }
        pista.push_back({ current_time, msg.voo });
    }

    // Imprime a tabela de mensagens do aeroporto atual
    printTabela(pista, rank);

    MPI_Finalize();
    return 0;
}