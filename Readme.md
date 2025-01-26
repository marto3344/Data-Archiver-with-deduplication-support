Проектът използва CMake за build. За да се компилира правилно проектът, създайте папка 'build'(ако няма такава) в текущата директория. Навигирайте до тази папка и стартирайте процеса ("cmake ..") . При нужда посочете експлицитно коя build система да изплозва CMake (например: cmake .. -G "MinGW Makefiles"). Ако искате да компилирате и тестовете изполвайте флага: "-DBUILD_TESTS=ON" (напр. "cmake ..cd -G "MinGW Makefiles" -DBUILD_TESTS=ON"). Компилирайте кода с командата спрямо build системата, която сте използвали (напр. mingw32-make). CMake ще създаде главния "executable" файл в същата папка с името "backup.exe". Тестовоте се изпълняват от поддиректорията "test", като файлът е именуван "tests.exe" . 