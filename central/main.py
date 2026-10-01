"""
Central de Controle de Irrigação Automatizada
Ponto de entrada (Entrypoint) principal do sistema.
"""
import config.settings as cfg
from core.engine import IrrigationEngine

def main():
    engine = IrrigationEngine()
    engine.start()

if __name__ == "__main__":
    main()
