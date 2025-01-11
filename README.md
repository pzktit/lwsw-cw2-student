# **Linux w Systemach Wbudowanych**

# **2. Konfiguracja linii GPIO jako wyjść w systemach Raspberry Pi i Linux**

**Piotr ZAWADZKI**

## 2.5 Zadania do wykonania

Elementy interfejsu aplikacji

- **GPIO 6**: Zarządzane przez nakładkę `gpio-led` (kontrola diody LED za pomocą sysfs).
- **GPIO 17**: Podświetlenie ekranu zarządzane bezpośrednio przy użyciu `libgpiod` (narzędzia `gpiod`).
- **GPIO 4**: Przycisk chwilowy normalnie otwarty — służy do zmiany stanu aplikacji. Obsługa przycisków wejściowych jest jednym z elementów następnego cwiczenia.

<div style="text-align: center;">

![Rys. 2.1 Ilustracja połączeń peryferiów wykorzystywanych w ćwiczeniu](schemat-cw2.png)

</div>

### **Zadanie 1: Sprawdź stany pinów za pomocą `pinctrl`**

**Cel**: Użyj narzędzia `pinctrl`, aby sprawdzić aktualny stan pinów oraz dostępne alternatywne funkcje dla GPIO 6 i GPIO 17.

- Zidentyfikuj kontroler pinów

  ```bash
  ls /sys/kernel/debug/pinctrl/
  ```

  Następnie użyj `rpi`.

- Sprawdź stan pinów dla GPIO 6

- Wylistuj dostępne funkcje dla GPIO 6

- Powtórz dla GPIO 17

<div style="background-color: yellow; color:red; padding: 2px;">
Wykonaj zrzut ekranu (2.1.1) zawierający komendy sesji terminala.
</div>

### **Zadanie 2: Pobierz konfigurację linii za pomocą `gpioinfo`**

**Cel**: Użyj `gpioinfo` z narzędzi `libgpiod`, aby uzyskać szczegółowe informacje o liniach GPIO 6 i 17.

- Użyj `gpioinfo`, aby wylistować wszystkie linie GPIO

- Uzyskaj informacje tylko o GPIO 6 i GPIO 17

- Spróbuj kontrolować GPIO 6 za pomocą `gpioset`. Jaka jest właściwa metoda kontroli tej linii?

<div style="background-color: yellow; color:red; padding: 2px;">
Wykonaj zrzut ekranu (2.2.1) zawierający komendy sesji terminala.
</div>

### **Zadanie 3: Steruj i sprawdzaj stan linii za pomocą `gpioset` oraz `gpioget`/`gpiomonitor`**

**Cel**: Wymuś stan GPIO 17 za pomocą `gpioset` i sprawdź wynik przy użyciu `gpioget`.

- Ustaw GPIO 17 jako wyjście i ustaw stan wysoki

- Zweryfikuj stan linii za pomocą `gpioget`

- W drugim terminalu monitoruj stan za pomocą `gpiomonitor`

- Ustaw GPIO 17 na niski stan

- Ponownie zweryfikuj stan linii

- Steruj GPIO 17 z ustawieniem trwałym

  Domyślnie `gpioset` ustawia linię i utrzymuje ją tylko tak długo, jak długo działa polecenie. Ustaw trwałość za pomocą opcji `--mode=exit`.

<div style="background-color: yellow; color:red; padding: 2px;">
Wykonaj zrzut ekranu (2.3.1) zawierający komendy sesji terminala.
</div>

### **Zadanie 4: Kontroluj diodę LED zarządzaną przez nakładkę `gpio-led` za pomocą Sysfs**

**Cel**: Użyj interfejsu sysfs do kontrolowania diody LED podłączonej do GPIO 6, zarządzanej przez nakładkę `gpio-led`.

- Zlokalizuj katalog sysfs dla diody LED za pomocą `ls`

- Włącz diodę LED

- Wyłącz diodę LED

- Sprawdź stan diody LED

- Ustaw diodę LED na miganie za pomocą trybu timer. Zmień czas dla stanów on/off diody LED. Następnie wróć do trybu manualnego.

<div style="background-color: yellow; color:red; padding: 2px;">
Wykonaj zrzut ekranu (2.4.1) zawierający komendy sesji terminala.
</div>

### **Zadanie 5: Napisz skrypt do sterowania podświetleniem**

**Cel**: Stwórz skrypt `toogle_backlight.sh`, który będzie przełączał podświetlenie ekranu podłączone do GPIO 17 co sekundę _Wskazówka_: wykorzystaj komendę `gpioset --mode=exit`.

- Utwórz pusty plik `toogle_backlight.sh` i nadaj mu uprawnienia wykonywania.

- Uruchom skrypt

  <div style="background-color: yellow; color:red; padding: 2px;">
  Wykonaj zrzuty ekranu (2.5.x) zawierające kod skryptu oraz komendy sesji terminala.
  </div>

### **Zadanie 6: Praca z API C++**

- Uruchom załączoną aplikację `cw2-prof` na maszynie `target`. Obserwuj jej zachowanie podczas krótkiego i długiego naciśnięcia przycisku.
- Skopiuj na `target` archiwum `cw2-student.tar` i rozpakuj je tam.
- Otwórz wynikowy folder zdalnie za pomocą Visual Studio Code uruchomionego na hoście.
- Zbuduj projekt. Zweryfikuj, czy wynikowa aplikacja działa. Zauważ różnice w jej zachowaniu podczas krótkiego naciskania klawisza.
- W miejscach oznaczonych jako `TODO` uzupełnij kod, aby naśladować zachowanie `cw2-prof`. _Proszę nie usuwać oryginalnych komentarzy._

<div style="background-color: yellow; color:red; padding: 2px;">
Wykonaj zrzuty ekranu (2.6.x) zawierające uzupełnione fragmenty kodu źródłowego. Zrzut zamieść w sprawozdaniu.
</div>
