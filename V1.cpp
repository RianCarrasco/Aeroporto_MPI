#include <iostream>
#include "mpi.h"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

#define D 1 // Variável que não sei qual é

using namespace std;

const int TAG = 0;
const int REQ_TAG = 1;
const int ACK_TAG = 2;

struct Voo
{
    int codigo;
    int origem;
    int destino;   // rank destino
    int horario;   // qual tempo lógico ele deve ir
    int tempo_voo; // tempo que a mensagem ficará no ar
    bool is_pouso;
};

int Lamport(int pi)
{
    return (pi * 2) + D;
}

struct Mensagem
{
    int lc; // Relógio lógico
    Voo voo;
};

void printTabela(const vector<Mensagem> &mensagens, int rank)
{
    cout << "Tabela: Configuração para o aeroporto (" << rank + 1 << ")" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    cout << "| Código  | Origem  | Horário de Chegada | Tempo de Voo |" << endl;
    cout << "| Pousos  |---------|--------------------|--------------|" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    for (const Mensagem &msg : mensagens)
    {
        if (msg.voo.is_pouso)
        {
            cout << "| " << setw(7) << msg.voo.codigo << " | " << setw(7) << msg.voo.origem << " | "
                 << setw(16) << msg.voo.horario << " | " << setw(13) << msg.voo.tempo_voo << " |" << endl;
        }
    }
    cout << "+---------+---------+------------------+---------------+" << endl;
    cout << "| Decola. | Destino | Horário de Partida | Tempo de Voo |" << endl;
    cout << "+---------+---------+------------------+---------------+" << endl;
    for (const Mensagem &msg : mensagens)
    {
        if (!msg.voo.is_pouso)
        {
            cout << "| " << setw(7) << msg.voo.codigo << " | " << setw(7) << msg.voo.destino << " | "
                 << setw(16) << msg.voo.horario << " | " << setw(13) << msg.voo.tempo_voo << " |" << endl;
        }
    }
    cout << "+---------+---------+------------------+---------------+" << endl
         << endl
         << endl;
}

bool compareMensagens(Mensagem &a, Mensagem &b)
{
    if (a.voo.horario == b.voo.horario)
        if(a.voo.tempo_voo > b.voo.tempo_voo)
        {
            b.voo.tempo_voo++;
            return a.voo.tempo_voo;
        }
        else
        {
            a.voo.tempo_voo++;
            return b.voo.tempo_voo;
        }
    return a.voo.tempo_voo;
}

vector<Voo> getVoos()
{
    vector<Voo> decolagens;
    int quantidade;
    cout << "Digite a quantidade de decolagens (-1 para decolagens predefinidas)" << endl;
    cin >> quantidade;

    if (quantidade < 0)
        return {// codigo, origem, destino, horario, tempo_voo, is_pouso
                {1, 0, 1, 0, 3, false},
                {2, 1, 0, 0, 2, false},
                {3, 1, 0, 0, 3, false}};

    for (int i = 0; i < quantidade; ++i)
    {
        cout << "Voo ( " << i+1 << " )" << endl;
        Voo voo;
        voo.is_pouso = false; // Sempre decolagem neste contexto
        cout << "Digite o código do voo: ";
        cin >> voo.codigo;
        cout << "Digite origem do voo: ";
        cin >> voo.origem;
        cout << "Digite o destino do voo: ";
        cin >> voo.destino;
        cout << "Digite o horário do voo: ";
        cin >> voo.horario;
        cout << "Digite o tempo do voo: ";
        cin >> voo.tempo_voo;

        decolagens.push_back(voo);
    }

    return decolagens;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int lc = 0; // Relógio lógico
    vector<Voo> pousos;
    vector<Voo> decolagens;

    if(rank==0)
        decolagens = getVoos();

    int decolagens_size = decolagens.size();
    MPI_Bcast(&decolagens_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    decolagens.resize(decolagens_size);
    MPI_Bcast(decolagens.data(), decolagens_size * sizeof(Voo), MPI_BYTE, 0, MPI_COMM_WORLD);

    vector<Mensagem> mensagens;

    // Formando códigos de voos
    int decolagem_count = 1;
    string resultado;

    for (Voo &voo : decolagens)
    {
        if (voo.origem == rank)
        {
            lc++;
            resultado = to_string(((rank + 1) * 10) + decolagem_count); // Primeiro número é a origem, segundo é a quantidade de decolagens
             voo.codigo = atoi(resultado.c_str());
            //voo.codigo = to_string(rank + 1) + to_string(decolagem_count);
            decolagem_count++;

            Mensagem msg = {lc, voo};
            mensagens.push_back(msg);
        }
    }

    // Enviar decolagens para os destinos
    for (Mensagem &msg : mensagens)
    {
        if (msg.voo.origem == rank)
            MPI_Send(&msg, sizeof(Mensagem), MPI_BYTE, msg.voo.destino, TAG, MPI_COMM_WORLD);
    }

    // Receber pousos de outros aeroportos
    int total_pousos_recebidos = 0;
    int total_pousos_esperados = 0;
    for (const Voo &voo : decolagens)
    {
        if (voo.destino == rank)
            total_pousos_esperados++;
    }

    while (total_pousos_recebidos < total_pousos_esperados)
    {
        MPI_Status status;
        Mensagem msg;
        MPI_Recv(&msg, sizeof(Mensagem), MPI_BYTE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);

        if (msg.voo.destino == rank)
        {
            msg.voo.is_pouso = true;
            pousos.push_back(msg.voo);
            mensagens.push_back(msg);
            total_pousos_recebidos++;
        }
    }

    // Priorizar pousos e decolagens
    sort(mensagens.begin(), mensagens.end(), compareMensagens);

    // Simular o uso da pista obedecendo as prioridades
    vector<Mensagem> pista;
    int current_time = 0;

    for (const Mensagem &msg : mensagens)
    {
        if (msg.voo.horario > current_time)
            current_time = msg.voo.horario;
        else
            current_time++;
        pista.push_back({current_time, msg.voo});
    }

    // Imprime a tabela de mensagens do aeroporto atual
    printTabela(pista, rank);

    MPI_Finalize();
    return 0;
}