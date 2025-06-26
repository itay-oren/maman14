# Compilation macros
CC = gcc
CFLAGS = -ansi -Wall -pedantic -g # Flags
EXE_DEPS = assembler.o exporter.o translator.o template.o macro_handler.o macro_data_structure.o line_disassemble.o table.o datas.o # Dependencies

## Executable
assembler: $(EXE_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

assembler.o:  driver/assembler.c step_3_assemble/exporter.h
	$(CC) -c $(CFLAGS) -o $@ $<

exporter.o: step_3_assemble/exporter.c step_3_assemble/exporter.h step_2_code_tranlate/translator.h
	$(CC) -c $(CFLAGS) -o $@ $<

translator.o: step_2_code_tranlate/translator.c step_2_code_tranlate/translator.h helpers/template.h data_structures/tabel.h
	$(CC) -c $(CFLAGS) -o $@ $<

template.o: helpers/template.c helpers/template.h step_1_preprocessor/makro_handller.h
	$(CC) -c $(CFLAGS) -o $@ $<

macro_handler.o: step_1_preprocessor/makro_handller.c step_1_preprocessor/makro_handller.h step_1_preprocessor/macro_data_structures.h helpers/line_disasseble.h
	$(CC) -c $(CFLAGS) -o $@ $<

macro_data_structure.o: step_1_preprocessor/macro_data_structures.c step_1_preprocessor/macro_data_structures.h data_structures/datas.h
	$(CC) -c $(CFLAGS) -o $@ $<

line_disassemble.o: helpers/line_disasseble.c helpers/line_disasseble.h
	$(CC) -c $(CFLAGS) -o $@ $<

table.o: data_structures/tabel.c data_structures/tabel.h
	$(CC) -c $(CFLAGS) -o $@ $<

datas.o: data_structures/datas.c data_structures/datas.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o assembler
