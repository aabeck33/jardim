import sys
from pathlib import Path

# Adiciona a raiz do projeto central ao sys.path
sys.path.append(str(Path(__file__).resolve().parent.parent))

try:
    from hardware.lora import LoRaRcvCont
    print("[OK] Importacao de LoRaRcvCont bem-sucedida a partir de hardware.lora!")
    
    try:
        lora = LoRaRcvCont()
        print("[OK] Instanciacao bem-sucedida!")
    except Exception as e:
        print(f"[INFO] Instanciacao falhou como esperado no Windows (Serial): {e}")

except ImportError as e:
    print(f"[ERRO] Erro de importacao: {e}")
except Exception as e:
    print(f"[ERRO] Erro inesperado: {e}")
