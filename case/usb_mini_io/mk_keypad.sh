
n_cols=$(("$1"))
n_rows=$(("$2"))

die() {
	echo "Usage: $0 <number-of-columns> <number-of-rows>"
	echo 
	echo "Creates: keypad_<number-of-keys>_keys_{top,bottom}.stl"
	exit 1
}


if [ "$n_cols" -lt 1 ]; then
	echo "bad number of columns" >&2
	die
fi

if [ "$n_rows" -lt 1 ]; then
	echo "bad number of rows" >&2
	die
fi

if ! which openscad; then
	echo "openscad not found" >&2
	exit 1
fi

gen() {
	name="$1"
	n_keys="$2"
	out="$name"_"$n_cols"_"$n_rows".stl

	echo generating "$out" >&2
	openscad -o "$out" -D n_cols="$n_cols" -D n_rows="$n_rows" "$name".scad &
}

gen keypad "$n_keys"
gen keypad_top "$n_keys"
gen keypad_bottom "$n_keys"

wait
wait
wait
