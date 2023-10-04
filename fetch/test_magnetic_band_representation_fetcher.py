from magnon.fetch.magnetic_band_representation_fetcher import (
    fetch_wp_point_group_and_br,
    fetch_kvectors_and_ebrs,
)


def dump(t):
    print()
    print("*****")
    print("type: ", type(t))
    print("str(t): ", str(t))
    print("*****")
    print()

def main():
    point_group, br = fetch_wp_point_group_and_br("205.33", "4a")
    dump(point_group)
    dump(br)
    dump(list(br.values())[0])

    kvectors, ebrs = fetch_kvectors_and_ebrs("205.33")
    dump(kvectors)
    dump(ebrs)

if __name__ == "__main__":
    main()
