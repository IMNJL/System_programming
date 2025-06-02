#!/usr/bin/awk -f

BEGIN {
    start_time = strftime("%Y-%m-%d %H:%M:%S")
    file = ARGV[1]
    
    # Чтение файла и сбор всех слов
    while ((getline < file) > 0) {
        # Удаляем начальные/конечные пробелы и разбиваем на слова
        gsub(/^[ \t]+|[ \t]+$/, "")
        n = split($0, words, /[ \t]+/)
        
        for (i = 1; i <= n; i++) {
            if (words[i] != "") {
                buffer[++count] = words[i]
            }
        }
    }
    close(file)
    
    # Вывод времени и результата
    printf "[%s] ", start_time
    for (i = count; i >= 1; i--) {
        printf "%s ", buffer[i]
    }
    printf "\n"
    
    # Опционально: вывод времени завершения
    end_time = strftime("%Y-%m-%d %H:%M:%S")
    # printf "Processing time: %s -> %s\n", start_time, end_time


}

END {
    printf "Processing time: %.2f seconds\n", systime() - start_epoch
}

