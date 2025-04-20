# Subway Runner - Jogo Implementado em C

Este é um jogo simples de corrida implementado em **C**. O objetivo do jogo é evitar obstáculos enquanto coleta moedas. O jogador pode se mover entre três faixas, pular ou deslizar para evitar os obstáculos e coletar itens.

## Estruturas de Dados e Algoritmos

### **Estruturas de Dados**

O jogo utiliza várias estruturas de dados para representar os elementos do jogo e organizar a memória de maneira eficiente.

#### **Player (Jogador)**
Representa o personagem do jogador no jogo. Ele possui:
- **x, y**: coordenadas do jogador no mapa.
- **lane**: a faixa onde o jogador está.
- **isJumping, isSliding**: flags que indicam se o jogador está pulando ou deslizando.
- **score, coins**: pontuação e quantidade de moedas coletadas pelo jogador.

#### **Obstacle (Obstáculo)**
Representa um obstáculo na faixa. Cada obstáculo possui:
- **lane**: faixa onde o obstáculo está localizado.
- **type**: tipo de obstáculo.
- **active**: flag que indica se o obstáculo está ativo ou não.

#### **Collectible (Item Coletável)**
Representa itens que o jogador pode coletar (como moedas). Cada item possui:
- **lane**: faixa onde o item está.
- **type**: tipo do item.
- **collected**: flag que indica se o item foi coletado.

#### **Tile (Bloco do Mapa)**
Representa um bloco do mapa onde o jogador interage com obstáculos e itens. Cada bloco pode ter até **3 obstáculos** e **3 itens**.

### **Algoritmos Utilizados**

O jogo implementa alguns algoritmos simples, mas eficientes, para gerenciar a movimentação e interação entre o jogador, os obstáculos e os itens.

#### **Algoritmos de Busca**
Não há um algoritmo de busca complexo implementado no jogo. A busca de colisões e itens no mapa é feita de maneira **linear**, verificando cada elemento de forma sequencial. Para o caso de colisões, o jogo verifica todos os obstáculos em cada **tile** (bloco do mapa) de forma direta.

#### **Deslocamento e Atualização do Mapa**
A atualização do mapa é feita de maneira sequencial. O mapa é **shiftado** (deslocado) a cada ciclo de atualização, criando um movimento do "mapa" em direção ao jogador, fazendo com que o jogador tenha que interagir com novos obstáculos e itens conforme o jogo avança. Esse movimento é implementado no código por meio da função `shiftMap`, que remove a primeira posição do mapa e adiciona um novo bloco na última posição. A movimentação do mapa ocorre de forma **iterativa**, sem recursividade.

#### **Verificação de Colisão**
A verificação de colisões entre o jogador e os obstáculos é feita de forma **linear** em cada **tile** (bloco). A cada ciclo do jogo, o código verifica se algum obstáculo ativo colide com o jogador, ou seja, se o jogador está na mesma faixa e não está pulando ou deslizando. Isso é feito na função `checkCollision`.

#### **Coleta de Itens**
A coleta de itens também é realizada de forma **linear**, verificando cada item no **tile** correspondente e marcando-o como coletado se o jogador estiver na mesma faixa.

### **Ponteiros e Gerenciamento de Memória**

Em C, o uso de **ponteiros** é crucial para gerenciar a memória de forma eficiente. O código utiliza ponteiros para alocar dinamicamente memória para as estruturas de dados principais (jogador, obstáculos, itens e mapa). A função `malloc` é usada para alocar memória para as estruturas `Player`, `Obstacle`, `Collectible` e `Tile`, e o uso de ponteiros permite modificar diretamente os valores dessas estruturas sem criar cópias desnecessárias.

O gerenciamento de memória é feito manualmente, com as funções de **liberação de memória** como `free`, que são utilizadas para garantir que a memória alocada seja liberada após o término do jogo. Isso evita **vazamentos de memória** e garante a eficiência no uso dos recursos.

Exemplo de alocação dinâmica de memória para o mapa:

```c
g->map = (Tile**)malloc(sizeof(Tile*) * MAP_SIZE);
