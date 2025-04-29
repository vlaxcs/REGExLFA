# Limbaje Formale și Automate
> Minciunescu Vlad-Andrei <br>
> Grupa 151 <br>
> Facultatea de Matematică și Informatică <br>
> Universitatea din București

## Sarcini și implementare

- Limbaj de programare: C++20
- Programul constă în verificarea testelor prin executarea unor sarcini specifice, deci obiectul de bază al aplicației este <code>Tester</code>.
- Atenție! Programul folosește o bibliotecă externă pentru parsarea fișierelor JSON (<i>nlohmann_json</i>). Configurația CMakeLists.txt care ar trebui să garanteze link-uirea corectă este:
```
include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(REGExLFA PRIVATE nlohmann_json::nlohmann_json)
```

<hr>

- [x] Testarea programului folosind fișiere JSON
    - Clasa <code>tester.cpp</code> inițializează și rulează aceste teste (soon to be a singleton appObject).
        - <code>init()</code>: Obține datele despre testele existente în toate fișierele JSON din directorul <code>REGExLFA/tests</code>
        - <code>run()</code>: Accesează datele obținute despre fiecare test și compară rezultatele programului cu cele așteptate. Furnizează rezultatul comparației, iar la final șterge toate testele rulate.
- [x] Convertor de expresii regulate în notație postfixată
  - Clasa <code>PostfixConverter</code> pune la dispoziție <code>getPostfix(expresie))</code>, prin care se obține o conversie de la forma la infix la postfix.
    - La forma infix a expresiei regulate se adaugă simbolurile de concatenare.
    - Ulterior acestei modificări, se obține forma postfixată, calculată prin intermediul algoritmului lui Shunting-Yard).
- [ ] Construcția NFA prin algoritmul lui Thompson, folosind postfix
- [ ] Conversia NFA -> DFA prin subset construction
- [ ] Simulant de DFA