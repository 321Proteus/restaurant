#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <array>
#include <string>
#include <dirent.h>
#include <sys/types.h>

using namespace std;



// Uzyte struktury

struct Danie {
    int id;
    string nazwa;
    double cena;
    string kategoria;
};

struct Zamowienie {
    int id;
    vector<Danie>dania;
    double cena;
};



// Zmienne globalne

vector<Zamowienie>historia;
vector <Danie> menu;
string cmd = "cls"; // "cls" dla Windows, "clear" dla Mac OS i Linuksa




// Dzielenie stringa na wektor liczb

vector<int> split(string &str) {
    stringstream ss(str);
    vector<int> a;
    int el;
    while (ss >> el) {
        a.push_back(el);
    }
    return a;

}

// Sortowanie potraw malejaco wedlug ID

bool idsort(Zamowienie& a, Zamowienie& b) {
    return a.id > b.id;
}

// Wczytywanie menu

void wczytajMenu() {

    DIR *dir;
    struct dirent *en;
    dir = opendir("data/menu");

    vector<string>pliki;

    // Pobieranie listy kategorii - plikow z potrawami
    if (dir) {
        while ((en = readdir(dir)) != 0) {
            pliki.push_back(en->d_name);
        }
        closedir(dir);
    }

    // Wczytywanie kategorii
    for (auto el : pliki) {

        if (el != "." && el != "..") {

            fstream file("data/menu/" + el);

            while (!file.eof()) {

                Danie danie;
                string tmp; getline(file, tmp, ';');
                danie.id = stoi(tmp);

                getline(file, tmp, ';');
                danie.nazwa = tmp;

                getline(file, tmp);
                danie.cena = stod(tmp);

                danie.kategoria = el;

                menu.push_back(danie);

            }
            file.close();
        }

    }
}

// Wyswietlanie menu

void wyswietlMenu() {

    string kategoria = "";

    for (auto el : menu) {

        if (kategoria != el.kategoria) {
            kategoria = el.kategoria;
            cout << "\n" << kategoria.substr(0, kategoria.length() - 4) << "\n";
        }
        printf("%d. %s %.2f zl\n", el.id, el.nazwa.c_str(), el.cena);
    }
}

// Generowanie ID kolejnego zamowienia

int generujID() {
    sort(historia.begin(), historia.end(), idsort);
    return historia[0].id + 1;
}

// Wczytywanie informacji o potrawie na podstawie podanego ID

Danie znajdzDanie(int id) {

    Danie szukane;

    for (auto a : menu) {
        if (a.id == id) {
            szukane = a;
            return a;
        }
    }

    szukane.id = -1;
    szukane.nazwa = "Nie ma takiego dania";
    szukane.cena = 0.00;

    return szukane;

}

// Wczytywanie historii zamowien

void wczytajHistorie() {

    fstream in("data/orders.txt");
    historia.clear();

    while (!in.eof()) {

        Zamowienie el;
        string tmp;
        getline(in, tmp, ';');
        int id = stoi(tmp);

        string lista; getline(in, lista, ';');

        vector<int>numery = split(lista);

        vector<Danie>dania;

        for (int i=0;i<numery.size();i++) {
            Danie a = znajdzDanie(numery[i]);
            dania.push_back(a);
        }

        getline(in, tmp);
        double cena = stod(tmp);

        el.id = id;
        el.cena = cena;
        el.dania = dania;

        historia.push_back(el);

    }

   in.close();

}

// Wyswietlanie historii zamowien

void wyswietlHistorie(int ilosc) {

    wczytajHistorie();

    sort(historia.begin(), historia.end(), idsort);

    if (!ilosc) {
        for (auto zamowienie : historia) {
            printf("ID: %d\n", zamowienie.id);
            cout << "Dania: ";
            for (auto el : zamowienie.dania) cout << el.id << ' ';
            printf("\nKwota: %.2f\n", zamowienie.cena);
        }
    }

    for (int i=0;i<ilosc;i++) {

        Zamowienie zamowienie = historia[i];
        printf("ID: %d\n", zamowienie.id);
        cout << "Dania: ";
        for (auto el : zamowienie.dania) cout << el.id << ' ';
        printf("\nKwota: %.2f\n", zamowienie.cena);
    }

}

// Wyswietlanie powitania

void wyswietlNazwe() {

    ifstream odczyt;
    odczyt.open("data/glowne.txt");
    string a;
    while(getline(odczyt,a)) cout << a << endl;

}

// Dodawanie zamowienia

void dodajZamowienie() {

    ofstream fout;
    fout.open("data/orders.txt", ios::app | ios::ate);

    Zamowienie zam;

    zam.id=generujID();

    int n=0,id=1;
    double w=0;
    while(n<10)
    {
        cout<<"Prosze podac numer id dania: ";
        cin>>id;

        if (id == 0) break;

        Danie b = znajdzDanie(id);
        if (b.id == -1)
        {
            cout << "\nBlad: nie znaleziono dania o podanym ID\n";
            return;
        }
        else
        {
            printf("Wybrano: %s\n", b.nazwa.c_str());
            w += b.cena;
            zam.dania.push_back(b);
        }

        n++;
    }
    zam.cena=w;
    // wyswietlanie zamowienia
    printf("Wybrano lacznie %d dania:\n",zam.dania.size());
    printf("Rachunek: %.2f\n", zam.cena);
    cout<<"Prosze wybrac odpowiednia opcje\n"
    <<"[0] - zaakceptuj zamowienie\n"
    <<"[1] - zrezygnuj z zamowienia";
    cin>>n;
    // zapis do historii
    if(n==0)
    {
        fout << '\n';
        fout<<zam.id<<';';
        for(int i=0;i<zam.dania.size();i++){ fout<<zam.dania[i].id<<" "; }
        fout<<';';
        fout<<zam.cena;
    }
}

int interfejs() {

    wyswietlNazwe();

    int b = 1; cin >> b;

    if (b == 0) return 0;

    else if (b == 1) {

        system(cmd.c_str());
        wyswietlMenu();

            cout << endl << "Aby wrocic wcisnij 5." << endl;
            int g; cin >> g;
            if(g == 5){
                system(cmd.c_str());
                interfejs();
            }
    }

    else if (b == 2) {

        system(cmd.c_str());
        dodajZamowienie();

        cout << endl << "Aby wrocic wcisnij 5." << endl;
        int g; cin >> g;
        if(g == 5){
            system(cmd.c_str());
            interfejs();
        }
    }

    else if (b == 3) {

        system(cmd.c_str());
        wyswietlHistorie(0);

        cout << endl << "Aby wrocic wcisnij 5." << endl;
        int g; cin >> g;
        if(g == 5){
            system(cmd.c_str());
            interfejs();
        }

    }
    return 0;
}

int main() {

    wczytajMenu();
    wczytajHistorie();
    interfejs();
}
