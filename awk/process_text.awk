BEGIN {
    total_files = 0
    total_lines = 0
    total_words = 0
    total_chars = 0

    print "| № строки | Исходная строка           | Измененная строка         |"
    print "--------------------------------------------------"
}

FNR == 1 {
    if (NR > 1) {
        print "--------------------------------------------------"
    }
    total_files++
    printf "\nобработка файла _  %s\n", FILENAME
}

{
    total_lines++
    total_chars += length($0) + 1  # +1 для символа новой строки
    total_words += NF

    orig_line = $0

    # Если меньше 5
    if (NF < 5) {
        printf "| %8d | %-40s | %-40s |\n", FNR, substr(orig_line, 1, 40), "Не хватает слов"
        next
    }

    if (FNR <= NF) {
        temp = $NF
        $NF = $FNR
        $FNR = temp
    }

    printf "| %8d | %-40s | %-40s |\n", FNR, substr(orig_line, 1, 40), substr($0, 1, 40)
}

END {
    print "--------------------------------------------------"
    print "\nстатистика:"
    print "обработано кол-во " total_files
    print "строк всего " total_lines
    print "слов всего " total_words
    print "символов всего " total_chars
}
