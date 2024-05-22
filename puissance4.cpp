#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <stdexcept>

extern "C" {
#include "list.h"
#include "board_puissance4.h"
}


Item *cur, *tmp;                       // Objets utilisés pendant le jeu
list_t openlist;                        // Liste des prochains mouvements possibles
int joueur = 1;                        // Pour suivre quel joueur est en train de jouer
int column;                            // Pour suivre dans quelle colonne la pièce a été placée

/*L'ordre des colonnes {3,4,5,6,2,1,0} est choisi pour prioriser les colonnes centrales, 
optimisant les stratégies de jeu en exploitant la centralité, la symétrie, et l'efficacité de 
l'exploration dans des algorithmes comme l'alpha-bêta pruning.
*/
int columns[] = {3,4,5,6,2,1,0}; // Ordre de recherche des colonnes

int nodesCount = 0; // Variable globale pour compter le nombre de nœuds traités
int nodesCount1 = 0;

int profondeur = 0; // Profondeur actuelle de l'arbre de jeu
int diff; // Variable pour stocker une différence, potentiellement utilisée pour évaluer des scénarios ou des scores



// Fonction pour calculer le maximum entre deux entiers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Fonction pour calculer le minimum entre deux entiers
int min(int a, int b) {
    return (a < b) ? a : b;
}

/*
Algo min et max : 
La fonction miniMax est une implémentation de l'algorithme MiniMax pour le jeu Puissance 4, 
qui explore les mouvements possibles sans élagage alpha-bêta. Elle alterne entre maximiser et minimiser les scores des 
joueurs à différentes profondeurs de recherche jusqu'à atteindre une condition terminale, comme un jeu terminé ou 
la profondeur maximale spécifiée. Les scores sont évalués en plaçant récursivement des pièces sur le plateau et
 en choisissant le meilleur score possible à chaque étape pour le joueur actuel.
*/

// min et max sans elagage alpha et beta 
/*
int miniMax(Item *node, int depth, int joueur){
   int eval, bestEval;

    if (depth == 0 || gameOver(node) ){
        if(gameOver(node))nodesCount++; 
            return evaluateBoard(node, joueur);
    }

      // Incrémente à chaque appel de miniMax, utiliser pour voir le nombre de noeuds


    if (joueur == -1){
        bestEval = 100000;
        
        for (int i = 0; i < COLUMNS; i++){
            column = columns[i];
            Item *child = getChildNode(node, column, joueur);
             if (child != NULL) { 
            eval = miniMax(child, depth-1, joueur*(-1));
            bestEval = min(bestEval,eval);

            if (depth == DEPTH)
            {
                child->f = eval;
                addLast(&openlist, child);
            }
             }
        }
        return bestEval;
    }
    else {
        bestEval = -100000;
         for (int i = 0; i < COLUMNS; i++){
            column = columns[i];
            Item *child = getChildNode(node, column, joueur);
            if(child != NULL){
            eval = miniMax(child, depth-1, joueur*(-1));
            bestEval = max(bestEval,eval);
            } 
    }
    return bestEval;
}

}
*/

/*
Algo min et max avec élagage Alpha Beta : 
La fonction miniMax avec élagage alpha-bêta est utilisée pour optimiser la recherche dans le jeu Puissance 4, 
alternant entre maximisation pour le joueur 1 et minimisation pour le joueur -1.
Cette version améliore l'efficacité en coupant les branches de l'arbre de recherche qui ne mèneront pas à de meilleures 
solutions que celles déjà évaluées, grâce à la mise à jour des bornes alpha et beta. Elle enregistre aussi les meilleurs 
coups dans openlist pour des analyses futures.
*/

int miniMax(Item *node, int depth, int alpha, int beta, int joueur) {
    int eval, bestEval;

    if (depth == 0 || gameOver(node)) {
       return evaluateBoard(node, -joueur);
    }

    nodesCount1++;  // Incrémente à chaque appel de miniMax

    if (joueur == -1) {  // Minimizing joueur
        bestEval = -100000;
        for (int i = 0; i < COLUMNS; i++) {
            int column = columns[i];
            Item *child = getChildNode(node, column, joueur);
            if (child != NULL) {
                eval = miniMax(child, depth - 1, alpha, beta, -joueur);
                bestEval = max(bestEval, eval);
                beta = max(beta, bestEval);
                  if (depth == profondeur)
                     {
                         child->f = eval;
                         addLast(&openlist, child);
                        } 
                if (beta <= alpha) {
                    break;  // Alpha cut-off
                }
            }
        }
        return bestEval;
    } else {  // Maximizing joueur
        bestEval = 100000;
        for (int i = 0; i < COLUMNS; i++) {
            int column = columns[i];
            Item *child = getChildNode(node, column, joueur);
            if (child != NULL) {
                eval = miniMax(child, depth - 1, alpha, beta, -joueur);
                bestEval = min(bestEval, eval);
                alpha = min(alpha, bestEval);
               
                if (alpha >= beta) {
                    break;  // Beta cut-off
                }
            }
        }
        return bestEval;
    }
}

//Fonction qui affiche la page d'acceuil avec un message de bienvenu et button startGame
void welcomeMessage(sf::RenderWindow &window) {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("imgFirst.png")) {
        throw std::runtime_error("Impossible de charger l'image de fond background.jpg");
    }

    sf::Sprite background(backgroundTexture);
    sf::Font font;
    if (!font.loadFromFile("Pacifico.ttf")) {
        throw std::runtime_error("Impossible de charger la police Pacifico.ttf");
    }

    sf::Text message("Bienvenue au Puissance 4!", font, 40);
    message.setFillColor(sf::Color::Black);
    sf::FloatRect messageBounds = message.getLocalBounds();
    message.setPosition((window.getSize().x - messageBounds.width) / 2, 20);

    sf::RectangleShape button(sf::Vector2f(150, 50));
    button.setFillColor(sf::Color::Green);
    button.setPosition(window.getSize().x - button.getSize().x - 20, (window.getSize().y - button.getSize().y) / 2); // Bouton à droite et centré verticalement

    // Positionner le texte dans le bouton
    sf::Text buttonText("START GAME", font, 20);
    buttonText.setFillColor(sf::Color::Black);
    sf::FloatRect buttonBounds = button.getGlobalBounds();
    buttonText.setPosition(buttonBounds.left + (buttonBounds.width - buttonText.getLocalBounds().width) / 2, buttonBounds.top + (buttonBounds.height - buttonText.getLocalBounds().height) / 2);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return;
                }
            }
        }

        // Animation du bouton lorsqu'il est survolé
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            button.setSize(sf::Vector2f(160, 60));
            button.setPosition(window.getSize().x - button.getSize().x - 20, (window.getSize().y - button.getSize().y) / 2); // Position ajustée lorsqu'il est survolé
        } else {
            button.setSize(sf::Vector2f(150, 50));
            button.setPosition(window.getSize().x - button.getSize().x - 20, (window.getSize().y - button.getSize().y) / 2); // Position initiale
        }

        window.clear();
        window.draw(background);
        window.draw(message);
        window.draw(button);
        window.draw(buttonText);
        window.display();
    }
}

sf::Color selectPlayerColor(sf::RenderWindow &window) {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("imgColor.png")) {
        throw std::runtime_error("Impossible de charger l'image de fond imgColor.png");
    }
    sf::Sprite background(backgroundTexture);

    sf::Font font;
    if (!font.loadFromFile("Pacifico.ttf")) {
        throw std::runtime_error("Impossible de charger la police Pacifico.ttf");
    }

    std::vector<sf::Color> colors = {sf::Color::White, sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Magenta, sf::Color::Cyan};
    std::vector<std::string> colorNames = {"blanc", "rouge", "vert", "bleu", "magenta", "cyan"};
    std::vector<sf::RectangleShape> colorRects;

    float rectWidth = 85;
    float rectHeight = 85;
    float spacing = 20;

    float totalWidth = colors.size() * rectWidth + (colors.size() - 1) * spacing;
    float startX = (window.getSize().x - totalWidth) / 2;

    for (size_t i = 0; i < colors.size(); ++i) {
        sf::RectangleShape rect(sf::Vector2f(rectWidth, rectHeight));
        rect.setFillColor(colors[i]);
        rect.setPosition(startX + i * (rectWidth + spacing), window.getSize().y / 2 - rectHeight / 2);
        colorRects.push_back(rect);
    }

    sf::Text message("Choisi une couleur !", font, 40);
    message.setFillColor(sf::Color::Black);

    float messageWidth = message.getLocalBounds().width;
    float messageX = (window.getSize().x - messageWidth) / 2;
    float messageY = window.getSize().y / 2 - rectHeight / 2 - 50;
    message.setPosition(messageX, messageY);

    sf::Color selectedColor = sf::Color::Black;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (size_t i = 0; i < colorRects.size(); ++i) {
                    if (colorRects[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedColor = colors[i];
                        window.clear();
                        return selectedColor;
                    }
                }
            }
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (size_t i = 0; i < colorRects.size(); ++i) {
            if (colorRects[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                colorRects[i].setSize(sf::Vector2f(rectWidth + 20, rectHeight + 20));
                colorRects[i].setPosition(startX + i * (rectWidth + spacing) - 10, window.getSize().y / 2 - rectHeight / 2 - 10);
            } else {
                colorRects[i].setSize(sf::Vector2f(rectWidth, rectHeight));
                colorRects[i].setPosition(startX + i * (rectWidth + spacing), window.getSize().y / 2 - rectHeight / 2);
            }
        }

        window.clear();
        window.draw(background);
        window.draw(message);
        for (const auto &rect : colorRects) {
            window.draw(rect);
        }
        window.display();
    }

    return selectedColor;
}

int chooseDifficulty(sf::RenderWindow &window) {
    int diff = -1;

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("imgChoice.png")) {
        throw std::runtime_error("Impossible de charger l'image de fond imgColor.png");
    }
    sf::Sprite background(backgroundTexture);

    sf::Font font;
    if (!font.loadFromFile("Pacifico.ttf")) {
        throw std::runtime_error("Impossible de charger la police Pacifico.ttf");
    }

    sf::Text message("Choisissez le niveau de difficultée:", font, 40);
    message.setFillColor(sf::Color::Black);
    message.setPosition((window.getSize().x - message.getLocalBounds().width) / 2, 50);

    std::vector<std::pair<std::string, sf::Color>> difficultyOptions = {
        {"Débutant", sf::Color::White},
        {"Intermédiaire", sf::Color::Green},
        {"Expert", sf::Color::Red}
    };

    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;

    float startY = 150;
    for (const auto &option : difficultyOptions) {
        sf::RectangleShape button(sf::Vector2f(200, 50));
        button.setFillColor(option.second);
        button.setPosition(50, startY);
        buttons.push_back(button);

        sf::Text text(option.first, font, 20);
        text.setFillColor(sf::Color::Black);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(button.getPosition().x + (button.getSize().x - textBounds.width) / 2, button.getPosition().y + (button.getSize().y - textBounds.height) / 2);
        buttonTexts.push_back(text);

        startY += 100;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (i == 0) {
                            diff = 1;
                            profondeur = 1;
                        } else if (i == 1) {
                            diff = 2;
                            profondeur = 3;
                        } else if (i == 2) {
                            diff = 3;
                            profondeur = 5;
                        }
                        return diff;
                    }
                }
            }
        }

        // Animation des boutons lorsqu'ils sont survolés
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (buttons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                buttons[i].setSize(sf::Vector2f(210, 60));
                buttons[i].setPosition(45, 145 + i * 100);
                sf::FloatRect textBounds = buttonTexts[i].getLocalBounds();
                buttonTexts[i].setPosition(buttons[i].getPosition().x + (buttons[i].getSize().x - textBounds.width) / 2, buttons[i].getPosition().y + (buttons[i].getSize().y - textBounds.height) / 2);
            } else {
                buttons[i].setSize(sf::Vector2f(200, 50));
                buttons[i].setPosition(50, 150 + i * 100);
                sf::FloatRect textBounds = buttonTexts[i].getLocalBounds();
                buttonTexts[i].setPosition(buttons[i].getPosition().x + (buttons[i].getSize().x - textBounds.width) / 2, buttons[i].getPosition().y + (buttons[i].getSize().y - textBounds.height) / 2);
            }
        }

        window.clear();
        window.draw(background);
        window.draw(message);
        for (size_t i = 0; i < buttons.size(); ++i) {
            window.draw(buttons[i]);
            window.draw(buttonTexts[i]);
        }
        window.display();
    }

    return diff; // Retourne -1 si la fenêtre est fermée sans sélection de difficulté
}

void endGameMessage(sf::RenderWindow &gameWindow, const std::string& message)
{
    // Chargement de l'image de fond
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("imgEnd.png")) {
        throw std::runtime_error("Impossible de charger l'image de fond imgColor.png");
    }
    sf::Sprite background(backgroundTexture);

    // Chargement de la police
    sf::Font font;
    if (!font.loadFromFile("Pacifico.ttf")) {
        throw std::runtime_error("Impossible de charger la police Pacifico.ttf");
    }

    // Effacer la fenêtre de jeu
    gameWindow.clear();

    // Création du texte du message
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setString(message);

    // Positionner le texte à droite et centré verticalement
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(gameWindow.getSize().x - textBounds.width - 10, (gameWindow.getSize().y - textBounds.height) / 2);

    // Déterminer la couleur du texte en fonction du message
    if (message.find("Félicitations") != std::string::npos) // Victoire
        text.setFillColor(sf::Color::Green);
    else if (message.find("Dommage") != std::string::npos) // Défaite
        text.setFillColor(sf::Color::Red);
    else // Égalité
        text.setFillColor(sf::Color::Black);

    // Dessiner le texte et l'image de fond
    gameWindow.draw(background);
    gameWindow.draw(text);
    gameWindow.display();

    // Attendre jusqu'à ce que la fenêtre soit fermée
    sf::Event event;
    while (gameWindow.isOpen())
    {
        while (gameWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                gameWindow.close();
        }
    }
}

class ChessBoard
{
private:
    sf::VertexArray hlines;
    sf::VertexArray vlines;
      sf::Texture backgroundTexture; // Texture pour l'image de fond
    sf::Sprite backgroundSprite;   // Sprite pour afficher l'image de fond

public:
    int w;
    int h;

    ChessBoard(int w, int h)
    {
        hlines.setPrimitiveType(sf::Lines);
        vlines.setPrimitiveType(sf::Lines);
        this->w = w;
        this->h = h;
        if (!backgroundTexture.loadFromFile("imgColor.png")) {
            throw std::runtime_error("Impossible de charger l'image de fond background.jpg");
        }
        backgroundSprite.setTexture(backgroundTexture);
    };

    void draw()
    {
        for (int i = 0; i < 6; i++)
        {
            hlines.append(sf::Vertex(sf::Vector2f(0, (i * h) / 6), sf::Color::Black));
            hlines.append(sf::Vertex(sf::Vector2f(w, (i * h) / 6), sf::Color::Black));
        }

        for (int i = 0; i < 7; i++)
        {
            vlines.append(sf::Vertex(sf::Vector2f((i * w) / 7, 0), sf::Color::Black));
            vlines.append(sf::Vertex(sf::Vector2f((i * w) / 7, h), sf::Color::Black));
        }
    };

    void display(sf::RenderWindow &window)
    {
         window.draw(backgroundSprite);
        window.draw(hlines);
        window.draw(vlines);
    };
};

class DotPlayer
{
private:
    sf::CircleShape cercle;
    float diametre;

public:
    DotPlayer(int diametre)
    {
        this->diametre = diametre;
        cercle.setPointCount(100);
        cercle.setRadius(diametre / 2);
    };

    void color(sf::Color colorPlayer){
        cercle.setFillColor(colorPlayer);

    };

    void draw(int x, int y)
    {
        cercle.setPosition(x - (diametre / 2), y - (diametre / 2));
    };

    void display(sf::RenderWindow &window)
    {
        window.draw(cercle);
    };

    void printCoup()
    {
        std::cout << "Au tour du cercle" << std::endl;
    };
};

class DotIA
{
private:
    sf::CircleShape cercle2;
    float diametre;

public:
    DotIA(int diametre)
    {
        this->diametre = diametre;
        cercle2.setPointCount(100);
        cercle2.setRadius(diametre / 2);
    };

    void color(sf::Color colorIA){
        cercle2.setFillColor(colorIA);
    };

    void draw(int x, int y)
    {
        cercle2.setPosition(x - (diametre / 2), y - (diametre / 2));
    };

    void display(sf::RenderWindow &window)
    {
        window.draw(cercle2);
    };

};

class Game
{
private:
    int size;

public:

    int *TabPlayer;
    int *TabIA;

    Game(int size)
    {
        this->size = size;
        TabPlayer = new int[size];
        for (int i = 0; i < size; i++)
        {
            TabPlayer[i] = -1;
        }
        TabIA = new int[size];
        for (int i = 0; i < size; i++)
        {
            TabIA[i] = -1;
        }

    };


    void AddPosition(int *T, int colonne, int line)
{
    bool alreadyExists = false;
    for (int i = 0; i < size - 1; i += 2)
    {
        if (T[i] == colonne && T[i + 1] == line)
        {
            alreadyExists = true;
            break;
        }
    }

    if (!alreadyExists)
    {
        for (int i = 0; i < size - 1; i += 2)
        {
            if (T[i] == -1 && T[i + 1] == -1)
            {
                T[i] = colonne;
                T[i + 1] = line;
                break;
            }
        }
    }
};


    void drawIA(DotIA circle, sf::RenderWindow &window)
    {
        for (int i = 0; i < size; i += 2)
        {
            if (TabIA[i] != -1 && TabIA[i + 1] != -1)
            {
                circle.draw(TabIA[i], TabIA[i + 1]);
                circle.display(window);
            }
        }

    };

    void drawPlayer(DotPlayer circle, sf::RenderWindow &window)
    {
        for (int i = 0; i < size; i += 2)
        {
            if (TabPlayer[i] != -1 && TabPlayer[i + 1] != -1)
            {
                circle.draw(TabPlayer[i], TabPlayer[i + 1]);
                circle.display(window);
            }
        }

    };

    void printTab(int * T){
        for (int i = 0; i < size; i++)
        {
            std::cout << T[i] << ' ';
        }
        
    }
};

void iaTURN(sf::RenderWindow &gameWindow, ChessBoard &board, Game &game, DotIA &circleIA, DotPlayer &circlePlayer, int *TabIA, Item *boardP)
{

 int depth = profondeur;
    int bestEval = miniMax(cur, depth,-100000,100000,joueur);
    cur = popValue(&openlist, bestEval);
    cleanUpList(&openlist);
    


    for (int i = 0; i < SIZE; i++)
    {   
       

        
        if ( cur->board[i] == -1)
        {
             int column = (i % 7 ) * 100 + 50; 
        int row = ((i / 7 )) * 100 + 50;    
            game.AddPosition(game.TabIA, column, row);
        }
    }

    gameWindow.clear();
    board.draw();
    board.display(gameWindow);
    game.drawIA(circleIA, gameWindow); 
    game.drawPlayer(circlePlayer, gameWindow); 
    gameWindow.display();
}

void playerTurn(sf::RenderWindow &gameWindow, ChessBoard &board,  Game &game, DotPlayer &circlePlayer, DotIA &circleIA, int *TabPlayer)
{
    sf::Event event;
    bool clicked = false;

    while (!clicked && gameWindow.isOpen())
    {
        

        while (gameWindow.pollEvent(event))
        {
            
        gameWindow.clear();
        board.draw();
        board.display(gameWindow);
        game.drawIA(circleIA, gameWindow); 
        game.drawPlayer(circlePlayer, gameWindow);
        gameWindow.display();

            if (event.type == sf::Event::Closed)
                gameWindow.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;

                int column = mouseX / 100 ; 
                int line = mouseY / 100;   
                if (column >= 0 && column < COLUMNS &&joueur== 1) 
                {
                    tmp = getChildNode(cur, column, joueur);
                    if (!tmp)
                    {
                        std::cout << "Invalid move. Try again\n";
                    }
                    else
                    {
                        cur = tmp;
                        int colonne = column * 100 + 50;
                        int ligne = line * 100 + 50;
                        
                        game.AddPosition(TabPlayer, colonne, ligne);
                        clicked = true; 
                    }
                }
            }
        }

        
        gameWindow.clear();
        board.draw();
        board.display(gameWindow);
        game.drawIA(circleIA, gameWindow); 
        game.drawPlayer(circlePlayer, gameWindow);
        
        gameWindow.display();
    }
}

int main(){ 
   

    sf::RenderWindow gameWindow(sf::VideoMode(700, 600), "Puissance 4");
    welcomeMessage(gameWindow);
    sf::Color playerColor = selectPlayerColor(gameWindow);

    std::vector<sf::Color> availableColors = {sf::Color::White, sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan};
    availableColors.erase(std::remove(availableColors.begin(), availableColors.end(), playerColor), availableColors.end());
    sf::Color iaColor = availableColors[rand() % availableColors.size()];

    

    int diff = chooseDifficulty(gameWindow);
    if (diff == -1) {
        std::cerr << "Aucune difficulté choisie. Fermeture du jeu." << std::endl;
        return 1;
    }

    Game game(42);
    DotIA circleIA(90);
    DotPlayer circlePlayer(90);

    circleIA.color(iaColor);
    circlePlayer.color(playerColor);

    ChessBoard board(700, 600);


    initList(&openlist);
    cur = initGame();
    

    while (gameWindow.isOpen() && !gameOver(cur))
    {
        switch (joueur)
        {
        case 1:
            playerTurn(gameWindow, board, game, circlePlayer, circleIA, game.TabPlayer);
            break;
        case -1:
            sf::sleep(sf::seconds(1));
            iaTURN(gameWindow, board, game, circleIA, circlePlayer,  game.TabIA, cur);
            break;
        }

       joueur*= -1; 
        // printBoard(cur);
        
        // game.printTab(game.TabPlayer);
        // std::cout << std::endl;
        // game.printTab(game.TabIA);
        // std::cout << std::endl;

        gameWindow.clear();

        board.draw();
        board.display(gameWindow);
        game.drawIA(circleIA, gameWindow);
        game.drawPlayer(circlePlayer, gameWindow);

        gameWindow.display();
    }
    
    printBoard(cur);
    
    switch (evaluateBoard(cur, joueur))
    {
    case 1000000:
        endGameMessage(gameWindow, "Félicitations! Vous avez gagné!");
        break;
    case -1000000:
        endGameMessage(gameWindow, "Dommage! L'IA vous a battu.");
        break;
    default:
        endGameMessage(gameWindow, "EGALITE");
        break;
    }

    return 0;
};

