find ../ -iname "*.cpp" | \
	while read I; do
		python indentspacestotabs.py -n 4 -i "$I"
	done

find ../ -iname "*.h" | \
	while read I; do
		python indentspacestotabs.py -n 4 -i "$I"
	done
