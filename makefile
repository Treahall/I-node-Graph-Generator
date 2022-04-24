# Author: Trever Hall
# Date: 4/23/2022
# Description: Makefile for i-node graph.

CC = gcc
FLAGS = -pthread -Wall -Wextra

TARGET = generate-graph
GRAPHICS = graph.pdf

all: $(TARGET) $(GRAPHICS)

$(TARGET): %: %.c
	$(CC) $(FLAGS) $< -o $@
	./$@

$(GRAPHICS): %.pdf: %.gv
	dot -Tpdf:cairo $< -o $@

clean:
	rm $(TARGET)
	rm $(GRAPHICS)
	rm graph.gv
