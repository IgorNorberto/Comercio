#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUTOS 100
#define MAX_VENDAS 500

typedef struct {
    int codigo;
    char nome[50];
    float preco;
    int estoque;
} Produto;

typedef struct {
    int codigo;
    char nome[50];
    int quantidade;
    float valor;
    char tipo[20];
} HistoricoVenda;

Produto produtos[MAX_PRODUTOS];
HistoricoVenda historico[MAX_VENDAS];
int totalProdutos = 0;
int totalVendas = 0;
float totalVendido = 0.0f;
int unidadesVendidas = 0;

void registrarHistorico(int codigo, const char *nome, int quantidade, float valor, const char *tipo);

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int buscarProduto(int codigo) {
    for (int i = 0; i < totalProdutos; i++) {
        if (produtos[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

int quantidadeVendidaParaProduto(int codigo) {
    int total = 0;

    for (int i = 0; i < totalVendas; i++) {
        if (historico[i].codigo == codigo) {
            if (strcmp(historico[i].tipo, "VENDA") == 0) {
                total += historico[i].quantidade;
            } else if (strcmp(historico[i].tipo, "DEVOLUCAO") == 0) {
                total -= historico[i].quantidade;
            }
        }
    }

    return total > 0 ? total : 0;
}

void cadastrarProduto(void) {
    if (totalProdutos >= MAX_PRODUTOS) {
        printf("\nLimite de produtos atingido.\n");
        return;
    }

    Produto novo;
    printf("\n--- Cadastro de produto ---\n");
    printf("Codigo: ");
    scanf("%d", &novo.codigo);
    limparBuffer();

    if (buscarProduto(novo.codigo) != -1) {
        printf("Ja existe um produto com este codigo.\n");
        return;
    }

    printf("Nome: ");
    fgets(novo.nome, sizeof(novo.nome), stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    printf("Preco unitario: ");
    scanf("%f", &novo.preco);
    limparBuffer();

    printf("Estoque inicial: ");
    scanf("%d", &novo.estoque);
    limparBuffer();

    produtos[totalProdutos++] = novo;
    printf("\nProduto cadastrado com sucesso!\n");
}

void listarProdutos(void) {
    if (totalProdutos == 0) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    printf("\n--- Estoque atual ---\n");
    printf("%-8s %-20s %-12s %-8s %-12s\n", "COD", "NOME", "PRECO", "ESTOQUE", "VALOR TOTAL");
    for (int i = 0; i < totalProdutos; i++) {
        float valorTotal = produtos[i].preco * produtos[i].estoque;
        printf("%-8d %-20s R$ %-10.2f %-8d R$ %-10.2f\n",
               produtos[i].codigo,
               produtos[i].nome,
               produtos[i].preco,
               produtos[i].estoque,
               valorTotal);
    }
}

void venderProduto(void) {
    int codigo, quantidade, continuar = 1;
    float totalVenda = 0.0f;
    int totalItens = 0;

    printf("\n--- Venda multipla ---\n");
    printf("Digite os produtos da venda. Para finalizar, informe codigo 0.\n");

    while (continuar) {
        printf("\nCodigo do produto (0 para finalizar): ");
        scanf("%d", &codigo);
        limparBuffer();

        if (codigo == 0) {
            break;
        }

        int pos = buscarProduto(codigo);
        if (pos == -1) {
            printf("Produto nao encontrado.\n");
            continue;
        }

        printf("Quantidade: ");
        scanf("%d", &quantidade);
        limparBuffer();

        if (quantidade <= 0) {
            printf("Quantidade invalida.\n");
            continue;
        }

        if (produtos[pos].estoque < quantidade) {
            printf("Estoque insuficiente para %s. Disponivel: %d\n", produtos[pos].nome, produtos[pos].estoque);
            continue;
        }

        float valorItem = produtos[pos].preco * quantidade;
        produtos[pos].estoque -= quantidade;
        totalVenda += valorItem;
        totalItens += quantidade;
        registrarHistorico(codigo, produtos[pos].nome, quantidade, valorItem, "VENDA");

        printf("Item adicionado: %s x %d - R$ %.2f (preco unitario: R$ %.2f)\n", produtos[pos].nome, quantidade, valorItem, produtos[pos].preco);
    }

    if (totalItens == 0) {
        printf("\nNenhum item foi vendido.\n");
        return;
    }

    totalVendido += totalVenda;
    unidadesVendidas += totalItens;

    printf("\nVenda realizada com sucesso!\n");
    printf("Total da venda: R$ %.2f\n", totalVenda);
    printf("Total de itens vendidos: %d\n", totalItens);
}

void registrarHistorico(int codigo, const char *nome, int quantidade, float valor, const char *tipo) {
    if (totalVendas >= MAX_VENDAS) {
        printf("\nHistorico cheio. Nao foi possivel registrar mais movimentacoes.\n");
        return;
    }

    historico[totalVendas].codigo = codigo;
    strncpy(historico[totalVendas].nome, nome, sizeof(historico[totalVendas].nome) - 1);
    historico[totalVendas].nome[sizeof(historico[totalVendas].nome) - 1] = '\0';
    historico[totalVendas].quantidade = quantidade;
    historico[totalVendas].valor = valor;
    strncpy(historico[totalVendas].tipo, tipo, sizeof(historico[totalVendas].tipo) - 1);
    historico[totalVendas].tipo[sizeof(historico[totalVendas].tipo) - 1] = '\0';
    totalVendas++;
}

void reestocarProduto(void) {
    int codigo, quantidade;

    printf("\n--- Reestoque ---\n");
    printf("Codigo do produto: ");
    scanf("%d", &codigo);
    limparBuffer();

    int pos = buscarProduto(codigo);
    if (pos == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("Quantidade a adicionar: ");
    scanf("%d", &quantidade);
    limparBuffer();

    if (quantidade <= 0) {
        printf("Quantidade invalida.\n");
        return;
    }

    produtos[pos].estoque += quantidade;
    printf("Reestoque realizado com sucesso!\n");
    printf("Novo estoque de %s: %d\n", produtos[pos].nome, produtos[pos].estoque);
}

void devolverProduto(void) {
    int codigo, quantidade;

    printf("\n--- Devolucao de produto ---\n");
    printf("Codigo do produto: ");
    scanf("%d", &codigo);
    limparBuffer();

    int pos = buscarProduto(codigo);
    if (pos == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("Quantidade devolvida: ");
    scanf("%d", &quantidade);
    limparBuffer();

    if (quantidade <= 0) {
        printf("Quantidade invalida.\n");
        return;
    }

    int vendido = quantidadeVendidaParaProduto(codigo);
    if (quantidade > vendido) {
        printf("Devolucao invalida. Apenas %d unidade(s) ja foram vendidas deste produto.\n", vendido);
        return;
    }

    float valorDevolucao = produtos[pos].preco * quantidade;
    produtos[pos].estoque += quantidade;
    totalVendido -= valorDevolucao;
    if (unidadesVendidas >= quantidade) {
        unidadesVendidas -= quantidade;
    }
    registrarHistorico(codigo, produtos[pos].nome, quantidade, valorDevolucao, "DEVOLUCAO");

    printf("Devolucao registrada com sucesso!\n");
    printf("Valor da devolucao: R$ %.2f\n", valorDevolucao);
}

void mostrarHistorico(void) {
    if (totalVendas == 0) {
        printf("\nNenhuma venda ou devolucao registrada ainda.\n");
        return;
    }

    float valorTotalHistorico = 0.0f;

    printf("\n--- Historico de vendas e devolucoes ---\n");
    printf("%-8s %-20s %-10s %-12s %-12s\n", "COD", "PRODUTO", "TIPO", "QUANTIDADE", "VALOR");
    for (int i = 0; i < totalVendas; i++) {
        printf("%-8d %-20s %-10s %-12d R$ %-10.2f\n",
               historico[i].codigo,
               historico[i].nome,
               historico[i].tipo,
               historico[i].quantidade,
               historico[i].valor);

        if (strcmp(historico[i].tipo, "VENDA") == 0) {
            valorTotalHistorico += historico[i].valor;
        } else if (strcmp(historico[i].tipo, "DEVOLUCAO") == 0) {
            valorTotalHistorico -= historico[i].valor;
        }
    }

    printf("\nValor total acumulado no historico: R$ %.2f\n", valorTotalHistorico);
}

void mostrarResumo(void) {
    float valorEstoque = 0.0f;

    for (int i = 0; i < totalProdutos; i++) {
        valorEstoque += produtos[i].preco * produtos[i].estoque;
    }

    printf("\n--- Resumo do comercio ---\n");
    printf("Produtos cadastrados: %d\n", totalProdutos);
    printf("Valor total em estoque: R$ %.2f\n", valorEstoque);
    printf("Total vendido: R$ %.2f\n", totalVendido);
    printf("Unidades vendidas: %d\n", unidadesVendidas);
}

void exibirMenu(void) {
    printf("\n===== Sistema do Mercado =====\n");
    printf("1 - Cadastrar produto\n");
    printf("2 - Listar produtos\n");
    printf("3 - Registrar venda\n");
    printf("4 - Reestocar produto\n");
    printf("5 - Devolver produto\n");
    printf("6 - Historico de vendas\n");
    printf("7 - Resumo do comercio\n");
    printf("0 - Sair\n");
    printf("Escolha uma opcao: ");
}

int main(void) {
    int opcao;

    printf("Bem-vindo ao Sistema do Mercado!\n");

    do {
        exibirMenu();
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                cadastrarProduto();
                break;
            case 2:
                listarProdutos();
                break;
            case 3:
                venderProduto();
                break;
            case 4:
                reestocarProduto();
                break;
            case 5:
                devolverProduto();
                break;
            case 6:
                mostrarHistorico();
                break;
            case 7:
                mostrarResumo();
                break;
            case 0:
                printf("\nEncerrando o sistema. Ate logo!\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}
