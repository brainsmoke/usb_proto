
n_keys=$(("$1"))

if [ "$n_keys" -lt 1 ]; then
	echo "bad number of keys" >&2

	echo "Usage: $0 <number-of-keys>"
	echo 
	echo "Creates: keypad_<number-of-keys>_keys_{top,bottom}.stl"
	exit 1
fi

if ! which openscad; then
	echo "openscad not found" >&2
	exit 1
fi

gen() {
	name="$1"
	n_keys="$2"

	echo generating "$name"_"$n_keys".stl >&2
	openscad -o "$name"_"$n_keys".stl -D n_keys="$n_keys" "$name".scad &
}

gen keypad "$n_keys"
gen keypad_top "$n_keys"
gen keypad_bottom "$n_keys"

wait
wait
wait
