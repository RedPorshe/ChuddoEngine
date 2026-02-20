# Сборка ChuddoEngine

Краткие инструкции по сборке проекта под Windows с использованием Visual Studio.

Требования
- Visual Studio 2022/2026 (MSVC)
- C++17 или выше
- Отладочные/релизные конфигурации

Шаги
1. Откройте `ChuddoEngine.sln` в Visual Studio. Если файла решения нет, создайте новый `Empty C++ Project` и добавьте существующие файлы из `Source` и `Include`.
2. Убедитесь, что в свойствах проекта установлены следующие флаги:
   - C++ Language Standard: C++17
   - Runtime Library: Multi-threaded Debug DLL (MDd) для Debug
3. Постройте решение: Build -> Build Solution
4. Запустите `Debug` или `Release` исполняемый файл.

Примечания
- Логирование настраивается через макросы в `Include/Utils/Logger.h`.
- Если при сборке возникают ошибки из-за отсутствующих зависимостей — проверьте include paths в проекте.
