import sys
from pathlib import Path

# Adiciona a raiz do projeto central ao sys.path
sys.path.append(str(Path(__file__).resolve().parent.parent))

try:
    from ia import decide_irrigation
    from services import get_weather
    
    print("--- Teste 1: get_weather ---")
    weather = get_weather()
    print("Clima:", weather)
    
    print("\n--- Teste 2: decide_irrigation com weather_data = None ---")
    irrigar, tempo = decide_irrigation({"moisture": 10}, None)
    print(f"Resultado (None): Irrigar={irrigar}, Tempo={tempo}s")

    print("\n--- Teste 3: decide_irrigation com weather_data = [] (vazio) ---")
    irrigar, tempo = decide_irrigation({"moisture": 10}, [])
    print(f"Resultado (Vazio): Irrigar={irrigar}, Tempo={tempo}s")

    print("\n--- Teste 4: decide_irrigation com valores parciais None ---")
    weather_com_nones = [{"airTemperature": None, "humidity": None}]
    try:
        irrigar, tempo = decide_irrigation({"moisture": 10}, weather_com_nones)
        print(f"Resultado (Nones): Irrigar={irrigar}, Tempo={tempo}s")
    except Exception as e:
        print(f"Erro ao lidar com Nones individuais: {e}")

    print("\n[OK] Testes concluidos!")

except Exception as e:
    print(f"[ERRO] Erro nos testes: {e}")
