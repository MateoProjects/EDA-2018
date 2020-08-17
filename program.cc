#include "Player.hh"
#include <queue>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
typedef vector<char> colum;
typedef vector<vector<char> > Matriu;
typedef vector<vector<bool> > visitats;
typedef vector<int> VE;

#define PLAYER_NAME Provax64


// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  struct move {
	  int x;
	  int y;
	  vector <int> pos; // vector dels moviments
  };

  struct move_h{
    int x;
    int y;
    int orient;
    vector <int> pos;
  };

  /*---------  REVISA SI ES POSSIBLE MOURE HELICOPTER ---------*/

  bool revisa_move(int x , int y , const Matriu & planol , char moviment) {
    // x ja ve amb -3
    if(moviment == 'u') {
      for(int i = -2; i <= 2; ++i) if(planol[x][y + i] == 'X') return false; 
    }
    // y ja ve amb -3
    else if(moviment == 'l') {
      for(int i = -2; i <= 2; ++i) if(planol[x + i][y] == 'X') return false;
    }
    // y ja ve amb + 3
    else if (moviment == 'r') {
      for(int i = -2; i <= 2; ++i) if(planol[x + i][y] == 'X') return false;
    }

    else {
      for(int i = -2; i <= 2; ++i) if(planol[x][y + i] == 'X') return false;
    }

    return true;
  }




  /* --------- OMPLE STRUCTS DE MOVE ---------*/

  move omple_mov(move ant, int x , int y , int push) {
    move nou = ant;
    nou.x += x;
    nou.y += y;
    nou.pos.push_back(push);
    return nou;
  };

  /* --------- RETORNA SI ID ES TROBA DINS EL VECTOR ---------  */

  bool dicotomica_id(int id , const vector<int> & v , int esq, int dret) {
    if(esq > dret) return false;
    int mid = (esq + dret) / 2;
    if(v[mid] == id) return true;
    else if(id < v[mid]) return dicotomica_id(id , v , esq , mid -1);
    else if(id > v[mid]) return dicotomica_id(id , v , mid +1 , dret);
  }

  /* --------- CALCUL DEL MAPA --------- */
  
	Matriu planol(Data in) {
		vector <Post> punts = posts();
		Matriu plan (60, colum (60));
		for ( int i= 0; i < 60; ++i ) {
			for ( int j = 0; j < 60; ++j ) {
				int aux = what (i , j );
				if ( aux == 3 or aux == 4 ) plan[i][j] = 'X';
				else if(fire_time(i , j) != 0) plan[i][j] = 'X'; // revisa si hi ha foc
        else if(which_soldier(i , j) > 0) plan[i][j] = 'S';
        else plan[i][j] = '.';
			}
		}
		for ( int i = 0; i < 16; ++i ) {
			if ( punts[i].player != in.player ) plan[punts[i].pos.i][punts[i].pos.j] = 'P';
		}
		return plan;
	}

  /*--------- CALCUL DEL MAPA PER HELICOPTER---------  */

  Matriu planol_helicopter(Data in) {
    vector <Post> punts = posts();
    Matriu plan (60, colum (60 , '.'));
    for ( int i= 0; i < 60; ++i ) {
      for ( int j = 0; j < 60; ++j ) {
        int aux = what (i , j );
        if ( aux == 4 ) plan[i][j] = 'X';
        else if (which_helicopter(i,j) > 0) {
          int x = which_helicopter(i , j);
          if(x != in.id) {
            for(int ix = -2; ix <= 2 ; ix++){
              for(int iy = -2; iy <= 2; iy++) {
                plan[i+ix][iy+j] = 'X';
            }
           } 
          }
        }
      }
    }
    for ( int i = 0; i < 16; ++i ) {
      if ( punts[i].player != in.player ) plan[punts[i].pos.i][punts[i].pos.j] = 'P';

    }
    return plan;
  }


 /* --------- CALCUL AREA DE HELICOPTER PER LLENĂAMENT DE PARACAIGUDISTES --------- */

  move llensament_paracaigudistes(Data in , bool & possible) {
    // paso com a parametra el helicopter
    possible = false;
    Data heli = in;
    move aux;
    aux.x = heli.pos.i -2; // obtinc posicio i superior del helicopter
    aux.y = heli.pos.j -2; // obtinc posicio j esquerra del helicopter
    vector<move> candidats;
    // faig un map
    for(int i = 0; i < 2; ++i) {
      for(int j = 0; j < 2; ++j) {
        if(what(aux.x + i, aux.y +j) != 3 and fire_time(aux.x + i, aux.y + j) == 0) {
          possible = true;
          aux.x = aux.x + i;
          aux.y = aux.y + j;
          candidats.push_back(aux);
        } 
      }    
    }
    // s'evaluen els possibles candidats per tirar
    for(int i = 0; i < (int)candidats.size(); ++i) {
      if(candidats[i].x == in.pos.i and candidats[i].x == in.pos.j) return candidats[i];
    }

    return aux;
    
  } // tanca funcio 

   /*---------  CALCUL PER POSICIO VALIDA HELICOPTER ---------*/

  bool move_heli( int x, int y) {
    for ( int i=(-2); i<=2; ++i ) {
      for ( int j=(-2); j<=2; ++j ) {
        if (what(i+x,j+y) == 4) return false;
      }
    }
    return true;
  }



  /*--------- EVALUA QUIN HELICOPTER TIRAR PARACAIGUDISTES ---------*/ 

  int evalua_para(const Matriu & planol_sold , const VE & helicopter ) {
    move h1 = bfs_punts(planol_sold , data(helicopter[0]).pos.i , data(helicopter[0]).pos.j);
    move h2 = bfs_punts(planol_sold , data(helicopter[1]).pos.i , data(helicopter[1]).pos.j);
    return (h1.pos.size() > h2.pos.size() ? helicopter[1] : helicopter[0]);
  }

  /* --------- CALCUL PER TIRAR NAPALM  --------- */

  bool tirar_napalm(Data in) {
    // el tira si hi han mĂŠs soldats enemics que aliats.
    VE soldats = soldiers(me()); // tinc tots els ID dels soldats;
    move aux;
    aux.x = in.pos.i -2;
    aux.y = in.pos.j -2;
    int count_aliats = 0;
    int count_enemics = 0;
    for(int i = 0; i < 5; ++i) {
      for(int j = 0; j < 5; ++j){
        int id_sold = which_soldier(aux.x + i , aux.y +j);
        if(id_sold > 0) { // hi ha soldat
          if(not dicotomica_id(id_sold, soldats, 0 , soldats.size()-1)) ++count_enemics;
          else ++count_aliats;
        }

      }

    }
    if(count_aliats == 0 and count_enemics > 2) return true;
    else if(count_enemics -1 > count_aliats) return true; // minim han d'haver-hi dos mes per ser rentable
    else return false;
  }


  bool orientat(const move_h heli, int k , int & gir) {
    // la k em diu cap on vaig
    // amunt
    int orient = heli.orient;
    if(k == 0){
      if(orient == 2) return true;
      else if(orient == 0) {
        gir = 5;
        return false;
      }
      else if(orient == 1){
        gir = 5;
        return false;
      }

      else if(orient == 3) {
        gir = 6;
        return false;
      }  
    }
    // aball
    else if (k == 1){
      if(orient == 0) return true;
      else if(orient == 1) {
        gir = 6;
        return false;
      }
      else if(orient == 2){
        gir = 6;
        return false;
      }
      else if(orient == 3) {
        gir = 5;
        return false;
      }
    }

    // esquerra
    else if (k == 2){
      if(orient == 3)return true;
      else if(orient == 0) {
        gir = 6;
        return false;
      } 
      else if(orient == 1) {
        gir = 6;
        return false;
      } 
      else if(orient == 2) {
        gir = 5;
        return false;
      }
    }
    // dreta
    else if (k == 3){
      if(orient == 1) return true;
      else if(orient == 0) {
        gir = 5;
        return false;
      } 
      else if(orient == 2) {
        gir = 6;
        return false;
      }
      else if(orient == 3) {
        gir = 5;
        return false;
      }
    }

  }

  /*---------  REVISA SI HI HA PUNT SOTA AREA HELICOPTER ---------*/

  bool revisa_punt(int x , int y , const Matriu & planol) {
    
    for(int i = -1; i <= 1; ++i) {
      for(int j = -1; j <= 1; ++j) {
        if( (x + i) >= 0 and (x + i) < 60 and (y + j)  >= 0 and (y + j) < 60) {
          if(planol[x + i][y +j] == 'P') return true;
        }
      }
    }
    return false;
  }

  /*---------  CALCUL DEL BFS PER HELICOPTER ---------*/

  move_h bfs_helicopter(const Matriu & planol , int id) {
    vector <vector<bool> > vit(60 , vector<bool>(60, false));
    queue <move_h> q;
    Data in = data(id);
    move_h p;
    p.x = in.pos.i;
    p.y = in.pos.j;
    q.push(p);
    vector <int> direct;
    vit[p.x][p.y] = true;
    int x , y;
    while(not q.empty()) {
      p = q.front();
      x = p.x;
      y = p.y;
      direct = p.pos;

      q.pop();
      if(revisa_punt(x,y,planol)) return p;

      // amunt
      if(x > 0 and not vit[x -1][y] and revisa_move_v2(x , y, planol, 0)) {
        move_h seg;
        seg.pos = direct;
        seg.pos.push_back(0);
        seg.x = x - 1;
        seg.y = y;
       // if(planol[x -1][y] == 'P') return seg;
        q.push(seg);
        vit[x -1][y] = true;
      }
      // aball
       if(x < 59 and not vit[x +1][y] and revisa_move_v2(x , y, planol, 1)) {
        vit[x -1][y] = true;
        move_h seg;
        seg.pos = direct;
        seg.pos.push_back(1);
        seg.x = x +1;
        seg.y = y;
        //if(planol[x +1][y] == 'P') return seg;
        q.push(seg);
      }
      // esuerra 
       if(y > 0 and not vit[x][y -1] and revisa_move_v2(x , y, planol, 2)) {
        vit[x][y - 1] = true;
        move_h seg;
        seg.pos = direct;
        seg.pos.push_back(2);
        seg.x = x;
        seg.y = y -1;
        //if(planol[x][y -1] == 'P') return seg;
        q.push(seg);
      }
      // dreta
       if(y < 59 and not vit[x][y +1] and revisa_move_v2(x , y, planol, 3)) {
        vit[x][y +1] = true;
        move_h seg;
        seg.pos = direct;
        seg.pos.push_back(3);
        seg.x = x;
        seg.y = y + 1;
        //if(planol[x][y + 1] == 'P') return seg;
        q.push(seg);
      }
    }
    return p;

  }


  /*---------  CALCUL DEL BFS PER ELS SOLDATS SOLS AGAFAR PUNTS ---------*/
 
  move bfs_punts(const Matriu & planol , int x , int y) { 
    visitats vit(60,vector<bool> (60, false)); // matriu de visitats
    queue <move> q; 
    vector<int> v;
    int ids;
    move p;
    p.x = x;
    p.y = y;
    q.push(p);
    VE soldats = soldiers(me());
    while(not q.empty()) {
      p = q.front();
      q.pop();
      if(planol[p.x][p.y] == 'P') return p;

      else {
        for(int k = 0; k < 8; ++k) {
          int xx = p.x + I[k];
          int yy = p.y + J[k];
          
          if(xx >= 0 and xx < MAX and yy >= 0 and yy < MAX and planol[xx][yy] != 'X' and planol[xx][yy] != 'S' and not vit[xx][yy]) {
              move futura;
              futura.pos = p.pos;
              futura.pos.push_back(k + 1);
              futura.x = xx;
              futura.y = yy;
              q.push(futura);
              vit[xx][yy] = true;
          }
        }
      }
    }
    return p;
  }

  static constexpr int I[8] = {-1 , 1 , 0 , 0 , 1 , 1 , -1 , -1 };
  static constexpr int J[8] = { 0 , 0 , -1 , 1 , -1 , 1 , -1 , 1};

    // FALTA CONFIGURAR I AJUSTAR 
  bool revisa_move_v2(int x , int y , const Matriu & planol , int moviment) {
    // x ja ve amb -3
      if((x  + 3 >= 60 or x  -3 < 0 ) or (y +3 >= 60 or y -3 < 0)) return false;
      else {
        if(moviment == 0) {
          if(x -3> 0) {
            for(int i = -2; i <= 2; ++i)if(planol[x -3][y + i] == 'X') return false;
            /*if(planol[x-3][y-2] == 'X') return false;
            if(planol[x-3][y-1] == 'X') return false;
            if(planol[x-3][y-0] == 'X') return false;
            if(planol[x-3][y+1] == 'X') return false;
            if(planol[x-3][y+2] == 'X') return false;*/
            return true; 

            }
          return false;
        }
      
        // y ja ve amb -3
        else if(moviment == 2) {
          if(y -3>= 0) {
              for(int i = -2; i <= 2; ++i) if(planol[x +i][y -3] == 'X') return false;
              return true;

            /*if(planol[x-2][y-3] == 'X') return false;
            if(planol[x-1][y-3] == 'X') return false;
            if(planol[x+0][y-3] == 'X') return false;
            if(planol[x+1][y-3] == 'X') return false;
            if(planol[x+2][y-3] == 'X') return false;
            return true;*/
        }
          return false;

      }
        // y ja ve amb + 3
        else if (moviment == 3) {
        if(y +3 < 59) {
            /*if(planol[x-2][y+3] == 'X') return false;
            if(planol[x-1][y+3] == 'X') return false;
            if(planol[x+0][y+3] == 'X') return false;
            if(planol[x+1][y+3] == 'X') return false;
            if(planol[x+2][y+3] == 'X') return false;*/
            for(int i = -2; i <= 2; ++i) if(planol[x +i][y + 3] == 'X') return false;
            return true;
        }
        return false;
      }
        else {
         if(x + 3 < 59) {
            /*if(planol[x+3][y-2] == 'X') return false;
            if(planol[x+3][y-1] == 'X') return false;
            if(planol[x+3][y-0] == 'X') return false;
            if(planol[x+3][y+1] == 'X') return false;
            if(planol[x+3][y+2] == 'X') return false;*/
            for(int i = -2; i <= 2; ++i) if(planol[x + 3][y + i] == 'X') return false;
            return true; 
        }
        return false;
      }
      }
    
  }
  //const int FOREST = 1; const int GRASS = 2;
  void play_soldier(int id , const Matriu & mapa) {
    Data in = data(id);
    int i = in.pos.i;
    int j = in.pos.j;
    int player = in.player;
    move tomove; 
    //if(data(id).life > 10) {
      for (int k = 0; k < 8; ++k) {
        int ii = i + I[k];
        int jj = j + J[k];
        if (ii >= 0 and ii < MAX and jj >= 0 and jj < MAX) {
          int id2 = which_soldier(ii, jj);
          if (id2 and data(id2).player != player) {
              int x = what(i,j);
              int y = what(ii,jj);
              command_soldier(id, ii, jj); 
            return;
      //    }
        }
      }
    }
  
  tomove = bfs_punts(mapa,i,j);
    char movi = tomove.pos[0];
    if ( movi == 1 ) command_soldier(id,i-1,j);        /*Amunt */
    else if ( movi == 2 ) command_soldier(id,i+1,j);   /*aball */
    else if ( movi == 3 ) command_soldier(id,i,j-1);   /*esq */
    else if ( movi == 4 ) command_soldier(id,i,j+1);   /*dret */
    else if ( movi == 5 ) command_soldier(id,i+1,j-1); /*Diagonal descendent cap a la esq */
    else if ( movi == 6 ) command_soldier(id,i+1,j+1); /*Diagonal descendent cap a la dreta*/
    else if ( movi == 7 ) command_soldier(id,i-1,j-1); /*Diagonal ascendent cap a la esq*/
    else if ( movi == 8 ) command_soldier(id,i-1,j+1); /*Diagonal ascendent cap a la dret*/
  }

  void play_helicopter(int id) {
      Data in = data(id);
      int orient = in.orientation;
      Matriu mapa_heli = planol_helicopter(in); // Crea el mapa per l'helicopter
      if (in.napalm == 0 and tirar_napalm(in)) command_helicopter(id, NAPALM); 
      /*else if ( orient == 0 ) { // sud

        if (revisa_move_v2(in.pos.i +3 , in.pos.j , mapa_heli , 'd')) command_helicopter(id,2);
        else command_helicopter(id,3);
      }
      else if ( orient == 1 ) { // est
        if (revisa_move_v2(in.pos.i , in.pos.j +3, mapa_heli , 'r')) command_helicopter(id,2);
        else command_helicopter(id,3);
      }
      else if ( orient == 2 ) { // nord
        if ( revisa_move_v2(in.pos.i -3 , in.pos.j , mapa_heli , 'u')) command_helicopter(id,2);
        else command_helicopter(id,3);
      }
      else if ( orient == 3 ) { // oest
        if (revisa_move_v2(in.pos.i , in.pos.j -3, mapa_heli , 'l')) command_helicopter(id,2);
        else command_helicopter(id,3);
      }
    */
    else {
        // faig bfs
        move_h heli = bfs_helicopter(mapa_heli , in.id);
        // codi  amunt aball esquerra dreta - 1 2 3 4
        // South = 0 , East = 1 , North = 2,  West = 3
        // clock wise = 4 
        // counter clockwise = 3
        
        if(heli.pos.size() > 0) {
          int x = heli.pos[0];
          if(x == 0 ) {
            if(orient == 2) command_helicopter(id , FORWARD2);
            else if(orient == 0) command_helicopter(id, COUNTER_CLOCKWISE);
            else if(orient == 1) command_helicopter(id, COUNTER_CLOCKWISE);
            else if(orient == 3) command_helicopter(id, CLOCKWISE);
          }
        
        // vaig cap aball
        if(x == 1) {
          if(orient == 0) command_helicopter(id , FORWARD2);
          else if(orient == 1) command_helicopter(id, CLOCKWISE);
          else if(orient == 2) command_helicopter(id, CLOCKWISE);
          else if(orient == 3) command_helicopter(id, COUNTER_CLOCKWISE);
        }
        // vaig cap esquerra  
        if(x == 2){
          if(orient == 3) command_helicopter(id , FORWARD2);
          else if(orient == 0) command_helicopter(id, CLOCKWISE);
          else if(orient == 1) command_helicopter(id, CLOCKWISE);
          else if(orient == 2) command_helicopter(id, COUNTER_CLOCKWISE);
        }

        // vaig cap a dreta
        if(x == 3){
          if(orient == 1) command_helicopter(id , FORWARD2);
          else if(orient == 0) command_helicopter(id, COUNTER_CLOCKWISE);
          else if(orient == 2) command_helicopter(id, CLOCKWISE);
          else if(orient == 3) command_helicopter(id, COUNTER_CLOCKWISE);
        }
      }
    }


    
  }

  void throw_parachuter(int helicopter_id) {
    // We get the data of the helicopter...
    //Data in = data(helicopter_id);
    bool possible;
    move tirarp = llensament_paracaigudistes(data(helicopter_id), possible); // coord on van els paracaigudistes
    // ... and try to throw a parachuter, without even examining the land.
    if(possible) command_parachuter(tirarp.x, tirarp.y);
  }
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {

    int player = me();
    VE H = helicopters(player); // helicopters of my player
    VE S =    soldiers(player); //    soldiers of my player
    int helicopter_id = H[random(0, H.size()-1)];
    Data in_heli = data(helicopter_id);
    Matriu mapa_sold = planol(in_heli); // agafo el helicopter que tinc ja per fer calcul del mapa sencer per soldats
    //Matriu mapa_heli = planol_helicopter(in_heli); // Crea el mapa per l'helicopter
    if (data(H[0]).parachuters.size() >= data(H[1]).parachuters.size()) throw_parachuter(data(H[0]).id);
    else throw_parachuter(data(H[1]).id); 
    for (int i = 0; i < (int)H.size(); ++i) play_helicopter(H[i]); 
    for (int i = 0; i < (int)S.size(); ++i) play_soldier(S[i] , mapa_sold);
  }

};

constexpr int PLAYER_NAME::I[8];
constexpr int PLAYER_NAME::J[8];

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);