#include <iostream>
#include <fstream>

using namespace std;

string FILE_NAME = "texto.txt"; // nome do arquivo externo.
char DELIMITADOR = ';'; // informe o caractere que delimita os dados.

int INFINITO = 1000000000; //infinito definido como 1000000000 pois é um numero alto, e acredito que não dê problemas
ifstream file;

/*************************************************************
 * FUNCOES DE LEITURA DE ARQUIVO
**************************************************************/
int *split(string texto, char delimitador, int quantidade_itens){
    string text = texto;
    int *retorno = new int[quantidade_itens];

    for(int i = 0; i < quantidade_itens; i++){
        retorno[i] = stoi(text.substr(0, text.find(delimitador)));
        text = text.substr(text.find(delimitador) + 1, text.size() - text.find(delimitador));
    }

    return retorno;
}

void open_file(){
    file.open(FILE_NAME);
    if(!file.is_open()){
        cout << "Nao foi possivel abrir o arquivo!" << endl;
        file.close();
        exit(0);
    }
}

void close_file(){
    file.close();
}

int get_vertice(){
    string text;
    int vertice;
    file.seekg(0); 
    getline(file, text);
    vertice = stoi(text);
    return vertice;        
}

int get_arestas(){
    string text;
    int aresta;
    file.seekg(0); 
    getline(file, text);
    getline(file, text);
    aresta = stoi(text.substr(text.find(",") + 1, text.size() - text.find(",") + 1));
    return aresta;        
}

int **get_ligacoes(){
    int **ligacoes;
    int arestas = get_arestas();
    string text;
    file.seekg(0); 
    getline(file, text);
	getline(file, text);
    ligacoes = new int*[arestas];
    for (int i = 0; i < arestas; i++){
        ligacoes[i] = new int[3];
    }
    for (int i = 0; i < arestas; i++){
        getline(file, text);

        ligacoes[i] = split(text, DELIMITADOR, 3); //3 pois é a quantidade de dados
    }
    return ligacoes;
}

/*************************************************************
 * FUNCOES PARA MATRIZES
**************************************************************/
int **criar_matriz(int linha, int coluna){
    int **matriz;
    matriz = new int*[linha];
    for (int i = 0; i < linha; i++){
        matriz[i] = new int[coluna];
    }

    for (int i = 0; i < linha; i++){
        for (int j = 0; j < coluna; j++){
            matriz[i][j] = 0;
        }
    }

    return matriz;
}

void imprimir_matriz(int **matriz, int linha, int coluna){
    for (int i = 0; i < linha; i++){
        for (int j = 0; j < coluna; j++){
            cout << matriz[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

int soma_matriz(int **matriz, int linha, int coluna){
    int soma = 0;
    for(int i = 0; i < linha; i++){
        for(int j = 0; j < coluna; j++){
            soma += matriz[i][j];
        }
    }
    return soma;
}

/*************************************************************
 * FUNCOES PARA ANALISE
**************************************************************/

int **preencher_matriz_peso(int **ligacoes, int vertice, int aresta){
    
    int **matriz;
    matriz = new int*[vertice];
    for (int i = 0; i < vertice; i++){
        matriz[i] = new int[vertice];
    }

    for (int i = 0; i < vertice; i++){
        for (int j = 0; j < vertice; j++){
            if(i == j){
                matriz[i][j] = 0;
            }else{
                matriz[i][j] = INFINITO;
            }
        }
    }

    for (int i = 0; i < aresta; i++){
        matriz[ligacoes[i][0] - 1][ligacoes[i][1] - 1] = ligacoes[i][2];
        matriz[ligacoes[i][1] - 1][ligacoes[i][0] - 1] = ligacoes[i][2];
    }   

    return matriz;
}

/*
    Algoritmo floydWarshall, usei como base o algoritmo presente em:
         https://pt.tutorialcup.com/interview/graph/floyd-warshall-algorithm.htm#:~:text=Algoritmo%20Floyd%20Warshall%20%C3%A9%20um,(infinito)%20em%20cada%20c%C3%A9lula.

    ****OBS: Não usar quanto houver ciclos com valor total negativo.
*/
int **floydWarshall (int **matriz_peso, int vertice)
{
    int **matriz_menor_caminho = criar_matriz(vertice, vertice);

    for (int i = 0; i < vertice; i++){
        for (int j = 0; j < vertice; j++){
            matriz_menor_caminho[i][j] = matriz_peso[i][j];
        }
    }

    for (int i = 0; i < vertice; i++){
        for (int j = 0; j < vertice; j++){
            for (int k = 0; k < vertice; k++){
                matriz_menor_caminho[j][k] = min(matriz_menor_caminho[j][k], matriz_menor_caminho[j][i] + matriz_menor_caminho[i][k]);
            }
        }
    }

    return matriz_menor_caminho;
}

int *maximos_exteriores(int **matriz_caminho_minimo, int vertice){
    int *maximo_por_vertice = new int[vertice];
    for (int i = 0; i < vertice; i++){
        maximo_por_vertice[i] = -INFINITO;
        for (int j = 0; j < vertice; j++){
            if (i != j && matriz_caminho_minimo[i][j] > maximo_por_vertice[i]){
                maximo_por_vertice[i] = matriz_caminho_minimo[i][j];
            }
        }
    }
    return maximo_por_vertice;
}

int *maximos_interiores(int **matriz_caminho_minimo, int vertice){
    int *maximo_por_vertice = new int[vertice];
    for (int i = 0; i < vertice; i++){
        maximo_por_vertice[i] = -INFINITO;
        for (int j = 0; j < vertice; j++){
            if (i != j && matriz_caminho_minimo[j][i] > maximo_por_vertice[i]){
                maximo_por_vertice[i] = matriz_caminho_minimo[j][i];
            }
        }
    }
    return maximo_por_vertice;
}

int *somatorio_dos_vertices_exterior(int **matriz_caminho_minimo, int vertice){

    int *somatorio_dos_vertices = new int[vertice];
    for (int i = 0; i < vertice; i++){
        somatorio_dos_vertices[i] = 0;
        for (int j = 0; j < vertice; j++){
            somatorio_dos_vertices[i] += matriz_caminho_minimo[i][j];
        }
    }

    return somatorio_dos_vertices;
}

int *somatorio_dos_vertices_interior(int **matriz_caminho_minimo, int vertice){

    int *somatorio_dos_vertices = new int[vertice];
    for (int i = 0; i < vertice; i++){
        somatorio_dos_vertices[i] = 0;
        for (int j = 0; j < vertice; j++){
            somatorio_dos_vertices[i] += matriz_caminho_minimo[i][j];
        }
    }

    return somatorio_dos_vertices;
}

int *minimos_exteriores(int **matriz_caminho_minimo, int vertice){
    int *minimo_por_vertice = new int[vertice];
    for (int i = 0; i < vertice; i++){
        minimo_por_vertice[i] = INFINITO;
        for (int j = 0; j < vertice; j++){
            if (i != j && matriz_caminho_minimo[i][j] < minimo_por_vertice[i]){
                minimo_por_vertice[i] = matriz_caminho_minimo[i][j];
            }
        }
    }

    return minimo_por_vertice;
}

int *minimos_interiores(int **matriz_caminho_minimo, int vertice){
    int *minimo_por_vertice = new int[vertice];
    for (int i = 0; i < vertice; i++){
        minimo_por_vertice[i] = INFINITO;
        for (int j = 0; j < vertice; j++){
            if (i != j && matriz_caminho_minimo[j][i] < minimo_por_vertice[i]){
                minimo_por_vertice[i] = matriz_caminho_minimo[j][i];
            }
        }
    }

    return minimo_por_vertice;
}

int indice_mediana(int *somatorio, int vertice){
    int indice_vertice;
    int aux = INFINITO;
    for(int i = 0; i < vertice; i++){
        if(somatorio[i] < aux){
            aux = somatorio[i];
            indice_vertice = i;
        }
    }

    return indice_vertice;
}

int indice_centro(int *maximos, int vertice){
    int indice_vertice;
    int aux = INFINITO;
    for(int i = 0; i < vertice; i++){
        if(maximos[i] < aux){
            aux = maximos[i];
            indice_vertice = i;
        }
    }

    return indice_vertice;
}

int indice_anticentro(int *minimos, int vertice){
    int indice_vertice;
    int aux = INFINITO;
    for(int i = 0; i < vertice; i++){
        if(minimos[i] > aux){
            aux = minimos[i];
                indice_vertice = i;
        }
    }

    return indice_vertice;
}

/*************************************************************
 * FUNCOES PARA MOSTRAR RESULTADOS
**************************************************************/
void imprimir_ligacoes(int **ligacoes, int arestas){

    cout << "\nLigacoes:\n" << endl;

    for(int i = 1; i <= 2; i++){
        cout << "v" << i << "\t";  
    }
    cout << "Peso";
    cout << endl;
    for(int i = 1; i <= 3; i++){
        cout << "--\t";  
    }
    cout << endl;

    for (int i = 0; i < arestas; i++){
        for (int j = 0; j < 3; j++){
            cout << ligacoes[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

void imprimir_matriz_peso(int **matriz_peso, int **ligacoes, int vertice, int aresta){

    cout << "\nMatriz de Pesos:\n" << endl;

    for(int i = 0; i < vertice; i++){
        cout << "\tv" << i + 1;
    }
    cout << endl;
    for(int i = 0; i < vertice; i++){
        cout << "v" << i+1 << "\t";
        for(int j = 0; j < vertice; j++){
            if(matriz_peso[i][j] == INFINITO){
                cout << "INF" << "\t";
            }else{
                cout << matriz_peso[i][j] << "\t";
            }
        }
        cout << endl;
    }  
    cout << endl;
}

void imprimir_matriz_menor_caminho(int **matriz_menor_caminho, int **ligacoes, int vertice, int aresta){

    cout << "\nMatriz de Menor Caminho:\n" << endl;

    for(int i = 0; i < vertice; i++){
        cout << "\tv" << i + 1;
    }
    cout << endl;
    for(int i = 0; i < vertice; i++){
        cout << "v" << i+1 << "\t";
        for(int j = 0; j < vertice; j++){
            if(matriz_menor_caminho[i][j] == INFINITO){ //buga mas n sei pq
                cout << "INF" << "\t";
            }else{
                cout << matriz_menor_caminho[i][j] << "\t";
            }
        }
        cout << endl;
    }  
    cout << endl;  
}

// Imprimir informações do caminho minimo do grafo
void imprimir_informacoes_grafo(int **matriz_caminho_minimo, int **ligacoes, int vertice, int aresta){
    int *max = maximos_interiores(matriz_caminho_minimo, vertice);
    int *sum = somatorio_dos_vertices_exterior(matriz_caminho_minimo, vertice);
    int *min = minimos_exteriores(matriz_caminho_minimo, vertice);
    int mediana = indice_mediana(sum, vertice);
    int centro = indice_centro(max, vertice);
    int anticentro = indice_anticentro(min, vertice);

    for(int i = 0; i < vertice; i++){
        cout << "\tv" << i + 1;
    }
    cout << "\t\tMax\tSum\tMin" << endl;

    for(int i = 0; i < vertice; i++){
        cout << "v" << i+1 << "\t";
        for(int j = 0; j < vertice; j++){
            if(matriz_caminho_minimo[i][j] == INFINITO){
                cout << "INF" << "\t";
            }else{
                cout << matriz_caminho_minimo[i][j] << "\t";
            }
        }
        cout << "\t"<< max[i] << "\t" << sum[i] << "\t" << min[i] <<  endl;
    }
    cout << endl; 
    cout << "Mediana :\tv" << mediana + 1 << endl;
    cout << "Centro :\tv" << centro + 1 << endl;
    cout << "Anticentro:\tv" << anticentro + 1 << endl;
}
// NÃO FUNCIONA

/*************************************************************
 * MAIN
**************************************************************/
int main(){
    open_file();
    int v = get_vertice();
    int a = get_arestas();
    int **l = get_ligacoes();
    close_file();
    
    cout << "vertice = " << v << endl;
    cout << "arestas = " << a << endl;
    imprimir_ligacoes(l, a);

    int **m_peso = preencher_matriz_peso(l, v, a);
    imprimir_matriz_peso(m_peso, l, v, a);

    int **m_menor_caminho = floydWarshall(m_peso,  v);
    imprimir_matriz_menor_caminho(m_menor_caminho, l, v, a);

    cout << "--------------------------------------" << endl;

    imprimir_informacoes_grafo(m_menor_caminho, l, v, a);

    cout << "--------------------------------------" << endl;


    return 0;
}