#include <iostream>
#include "taulell.h"

///////////////////////////////////////////
//// Implementació de la classe taulell ///

//---- Constructora. Serveix per declarar una variable
//---- de tipus "taulell" de mida n x n inicialitzada
//---- amb les fitxes inicials d'una partida de Reversi.
//---- Pre: n >=2
taulell::taulell(nat n) {
//PRE: Entra un natural n.
//POST:Es crea un taulle de n x n i sestableixen les
//fitxes inicials a partir del tamany del tauler.
    // crea taula de mida n x n

    taula = vector<vector<casella> >(n,vector<casella>(n,casella()));

    // inicialitza les 4 caselles centrals del taulell
    // si n és senar, les 4 caselles centrals queden una mica desplaçades amunt i a l'esquerra

    int mitat = n/2;
    taula[mitat-1][mitat-1].omple(casella::BLANCA);
    taula[mitat][mitat-1].omple(casella::NEGRA);
    taula[mitat-1][mitat].omple(casella::NEGRA);
    taula[mitat][mitat].omple(casella::BLANCA);
}


//---- Diu si les coord p estan dins dels limits del taulell.
bool taulell::dins_limits(coord p) const {
//PRE: Entra una coordenada.
//POST: Retorna si la coordenada es dins del taulell o no.
    bool dins_x = p.x >= 0 && p.x < taula.size();
    bool dins_y = p.y >= 0 && p.y < taula.size();
    return dins_x && dins_y;
    }


//---- Accedeix a la casella de la coord p del taulell.
casella& taulell::operator()(coord p) {
//PRE: Entra una coordenada.
//POST: Retorna el valor de la casella que estava a la coordenada.
    return taula[p.x][p.y];
}


//---- Escriu a la pantalla el contingut del taulell.
void taulell::mostra() const {
//PRE: Cert.
//POST: Mostra per pantalla el contingut del tauler.
    //INV: mostra el número entre 1 i el tamany de la taula
    for (int i = 0; i <= taula.size(); i++) {
        if (i != 0) cout<<" "<<i;
        else cout<<" ";
    }

    cout<<endl;
    //INV: i < taula.size()
    for (int i = 0; i < taula.size(); i++) {
      //INV: j < taula.size() i mostra el valor de la casella
        for (int j = 0; j < taula.size(); j++) {
            if (j == 0) cout<<i+1;
            casella c = taula[i][j];
            cout<<" ";
            cout<<c.mostra();
        }
        cout<<endl;
    }
}


//---- Escriu a la pantalla el contingut del taulell marcant amb '?'
//---- les caselles on es poden posar fitxes del color donat.
void taulell::mostra(int color) const {
//PRE: Entra un color per referencia.
//POST: Mostra per pantalla un interrogant en qualsevol posicio si
// en la posició es pot posar una fitxa del color donat.
    queue<coord> coordenades = coord_pot_jugar(color);
    if (coordenades.size() == 0) {
        mostra();
        return;
    }
    //INV: mostra el número entre 1 i el tamany de la taula
    for (int i = 0; i <= taula.size(); i++) {
        if (i != 0) cout<<" "<<i;
        else cout<<" ";
    }
    cout<<endl;
    //INV: i < taula.size()
    for (int i = 0; i < taula.size(); i++) {
        //INV: j < taula.size() i mostra el valor de la casella o un ? si pot realitzar algun moviment.
        for (int j = 0; j < taula.size(); j++) {
            if (j == 0) cout<<i+1;
            casella c = taula[i][j];
            cout<<" ";
            if( coord(i,j) == coordenades.front()) {
                if (c.valor() == casella::LLIURE) cout<<"?";
                else cout << c.mostra();
                coordenades.pop();
            }
            else cout<<c.mostra();
        }
        cout<<endl;
    }
}


//---- Avalua les fitxes del taulell:
//---- num_caselles_blanques - num_caselles_negres.
int taulell::avalua() const {
//PRE: Cert.
//POST: Llegeix totes les fitxes del tauler i retorna la resta
// de caselles blanques amb les negres.
    int blanques = 0, negres = 0;
    for (int i = 0; i < taula.size(); i++) {
        for (int j = 0; j < taula.size(); j++) {
            casella c = taula[i][j];
            if (c.valor() == casella::BLANCA) blanques++;
            else if(c.valor() == casella::NEGRA) negres++;
        }
    }
    return blanques - negres;
}


//---- Comprova si es pot girar desde la coordenada cini en la direcció d
//---- varies fitxes de l'adversari (al final hem de trobar el color donat).
//---- Retorna: girar (si es pot girar o no), c (coordenada final on s'ha trobat el color donat)
void taulell::es_pot_girar(coord cini, direccio d, int color, bool &girar, coord &c) const {
//PRE: Entran la coordenada inicial, direccio i color.
//POST: Es retorna a girar si es pot girar la fitxa i a c la coordenada final.
    bool colocable = false, acabat = false;
    coord coord_final;
    cini = cini+d.despl();

    if (!dins_limits(cini)) return;

    casella cas = taula[cini.x][cini.y];

    if (cas.valor() != casella::LLIURE && cas.valor() != color) {
        while(!acabat && !colocable) {
            cini = cini+d.despl();
            if (dins_limits(cini)) {
                cas = taula[cini.x][cini.y];
                if (cas.valor() == color) {
                    colocable = true;
                    coord_final = cini;
                }
                else if(cas.valor() == casella::LLIURE) acabat = true;
                else acabat = true;
            }
        }
    }
    c = coord_final;
    girar = colocable;
}


//---- Comprova si es pot posar una fitxa de color a la coordena c:
//---- Cal comprovar si en una de les 8 direccions es poden girar fitxes
//---- de l'adversari (de color diferent al color donat).
bool taulell::mov_possible(coord c, int color) const {
//PRE: Entran una coordenada i un color.
//POST: Retorna si es pot colocar la fitxa de color a la coordenada.
    casella cas = taula[c.x][c.y];

    if (cas.valor() != casella::LLIURE) return false;

    bool possible;
    direccio d;
    coord final;
    while (!d.is_stop() && !possible) {
        es_pot_girar(c, d, color, possible, final);
        if (!possible) ++d;
    }
    return possible;
}


//---- Comprova si el color pot jugar (la fitxa de color es pot col·locar en algun lloc).
bool taulell::pot_jugar(int color) const {
//PRE: Entra un color.
//POST: Retorna si aquest color te algun moviment possible per saber si pot jugar.
    int i = 0, j;
    bool colocable;

    while (i < taula.size() && !colocable) {
        j = 0;
        while(j < taula.size() && !colocable) {
            coord cord = coord(i,j);
            if (mov_possible(cord, color)) colocable = true;
            j++;
        }
        i++;
    }
    return colocable;
}

//---- Retorna una cua amb les coordenades a on el color pot jugar.
//---- S'encuen per fila creixent, i per la mateixa fila per columna creixent.
queue<coord> taulell::coord_pot_jugar(int color) const {
//PRE:  Entra un color.
//POST: Retorna una cua de coordenades la qual conte les coordenades on el jugador del color que ha entrat pot jugar.
    queue<coord> coordenades;
    for (int i = 0; i < taula.size(); i++) {
        for (int j = 0; j < taula.size(); j++) {
            coord cord = coord(i,j);
            bool pos = mov_possible(cord, color);
            cout<<"";
            if (pos) coordenades.push(cord);
        }
    }
    return coordenades;
}

//---- Canvia el color de les caselles des de la casella ci
//---- fins a la casella cf en la direcció d.
void taulell::gira_fitxes(coord ci, coord cf, direccio d) {
//PRE: Entran la coordenada inicial, la final i la direcció.
//POST: Gira totes les fitxes (les canvia de color) des de coordenada inicial fins a la final seguint la direcció.
    ci = ci+d.despl();
    while (!(ci == cf)) {
        casella cas = taula[ci.x][ci.y];
        cas.omple(-1*cas.valor());
        taula[ci.x][ci.y] = cas;
        ci = ci + d.despl();
    }
}


//---- Col·loca la fitxa de color a la coordena c i gira les fitxes necessàries segons regles d'Otelo
void taulell::posa_fitxa(coord c, int color) {
//PRE: Entra una coordenada i un color.
//POST: Coloca la fitxa del color a la coordenada.
    taula[c.x][c.y].omple(color);
}
