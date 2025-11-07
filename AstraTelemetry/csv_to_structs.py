import csv

CSV_FILE = "telemetry_data.csv"
OUTPUT_FILE = "telemetry_structs.cpp"

encoding_map = {
    "uint8": "uint8_t",
    "uint16": "uint16_t",
    "Float": "float",
    "boolean": "bool"
}

structs = {
    "prop_atomic": [],
    "valve_atomic": [],
    "test_atomic": []
}

with open(CSV_FILE, newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    next(csvfile)
    for row in reader:
        var_name = row["Variable Name"].strip()
        encoding = row["Encoding"].strip()
        cpp_type = encoding_map.get(encoding.lower(), "float")

        for struct_name in structs.keys():
            if struct_name in row and str(row[struct_name]).strip() in ["x"]:
                structs[struct_name].append(f"    {cpp_type} {var_name};")

with open("telemetry_structs.cpp", "w") as f:
    f.write("#include <cstdint>\n#include <string>\n\n")
    for struct_name, fields in structs.items():
        f.write(f"struct {struct_name} {{\n")
        for field in fields:
            f.write(f"{field}\n")
        f.write("};\n\n")




