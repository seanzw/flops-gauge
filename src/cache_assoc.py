import cpuinfo

def get_l1_cache_associativity():
    try:
        info = cpuinfo.get_cpu_info()
        print(info)
        cache_info = info.get('l1_data_cache', {})

        if not cache_info:
            return "L1 cache information not available."

        associativity = cache_info.get('associativity', 'Unknown')
        return f"L1 Cache Associativity: {associativity}"

    except Exception as e:
        return f"Error: {str(e)}"


if __name__ == "__main__":
    result = get_l1_cache_associativity()
    print(result)