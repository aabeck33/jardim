"""
Central de Controle de Irrigação Automatizada
Ponto de entrada (Entrypoint) principal do sistema.
"""
from core import IrrigationEngine

def main():
    engine = IrrigationEngine()
    engine.start()

if __name__ == "__main__":
    main()
