#include <iostream>
#include "mpi.h"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

using namespace std;

const int NUM_AEROPORTOS = 4; // Número de aeroportos
const int TAG = 0;
const int REQ_TAG = 1;
const int ACK_TAG = 2;

struct Voo {
    int codigo;
    int origem;
    int destino;
    int horario;
    int tempo_voo;
    bool is_pouso;
};

struct Mensagem {
    int lc; // Relógio lógico
    Voo voo;
};

void printTabela(const vector<Mensagem>& mensagens, int rank) {
    cout << "Tabela: Configuração para o aeroporto a" << rank + 1 << "." << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    cout << "| Código  | Origem  | Horário de Chegada | Tempo de Voo |" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    for (const auto& msg : mensagens) {
        if (msg.voo.is_pouso) {
            cout << "| " << setw(7) << msg.voo.codigo << " | " << setw(7) << msg.voo.origem << " | "
                << setw(16) << msg.voo.horario << " | " << setw(13) << msg.voo.tempo_voo << " |" << endl;
        }
    }
    cout << "+---------+---------+------------------+---------------+" << endl;
    cout << "| Código  | Destino | Horário de Partida | Tempo de Voo |" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    for (const auto& msg : mensagens) {
        if (!msg.voo.is_pouso) {
            cout << "| " << setw(7) << msg.voo.codigo << " | " << setw(7) << msg.voo.destino << " | "
                << setw(16) << msg.voo.horario << " | " << setw(13) << msg.voo.tempo_voo << " |" << endl;
        }
    }
    cout << "+---------+---------+------------------+---------------+" << endl;
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

    int lc = 0; // Relógio lógico

    vector<Voo> pousos;
    vector<Voo> decolagens = {
        {1, rank, (rank + 1) % NUM_AEROPORTOS, 0, 3, false},
        {2, rank, (rank + 2) % NUM_AEROPORTOS, 1, 4, false},
        {3, rank, (rank + 3) % NUM_AEROPORTOS, 3, 1, false},
        {0, rank, (rank + 1) % NUM_AEROPORTOS, 4, 2, false}
    };
/*
struct Voo {
    int codigo;
    int origem;
    int destino;
    int horario;
    int tempo_voo;
    bool is_pouso;
};
*/

    vector<Mensagem> mensagens;

    // Formando códigos de voos de acordo com a descrição
    int pouso_count = 1;
    int decolagem_count = 1;

    for (auto& voo : decolagens) {
        lc++;
        
        //voo.codigo = stoi(to_string(rank * 10) + to_string(decolagem_count));
        voo.codigo = rank * 10 + decolagem_count;
        decolagem_count++;
        Mensagem msg = { lc, voo };
        mensagens.push_back(msg);
    }

    // Define o tipo MPI para Mensagem
    MPI_Datatype mpi_msg_type;
    MPI_Aint offsets[2] = { offsetof(Mensagem, lc), offsetof(Mensagem, voo) };
    int lengths[2] = { 1, sizeof(Voo) };
    MPI_Datatype types[2] = { MPI_INT, MPI_BYTE };
    MPI_Type_create_struct(2, lengths, offsets, types, &mpi_msg_type);
    MPI_Type_commit(&mpi_msg_type);

    // Enviar decolagens para os destinos
    for (auto& msg : mensagens) {
        MPI_Send(&msg, 1, mpi_msg_type, msg.voo.destino, TAG, MPI_COMM_WORLD);
    }

    // Receber pousos de outros aeroportos
    for (int i = 0; i < size - 1; i++) {
        MPI_Status status;
        Mensagem msg;
        MPI_Recv(&msg, 1, mpi_msg_type, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
        msg.voo.is_pouso = true;
        pousos.push_back(msg.voo);
        mensagens.push_back(msg);
    }

    // Priorizar pousos e decolagens
    sort(mensagens.begin(), mensagens.end(), compareMensagens);

    // Simular o uso da pista obedecendo as prioridades
    vector<Mensagem> pista;
    int current_time = 0;
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
    //test
    //tet 2
    MPI_Type_free(&mpi_msg_type);
    MPI_Finalize();
    return 0;
}
