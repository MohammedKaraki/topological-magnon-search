#!/usr/bin/env bash

output_pdf_filename="$(pdflatex -output-directory=/tmp $1 | tail | grep -F 'Output written on' | sed -E 's/Output written on //' | sed -E 's/\.pdf .*$/.pdf/')";
mv ~/x.pdf ~/x.pdf.old;
cp "$output_pdf_filename" ~/x.pdf && nohup evince ~/x.pdf&
