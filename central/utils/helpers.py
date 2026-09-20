import json


def parse_json_safely(data_str: str) -> dict | None:
    """Tenta decodificar uma string JSON com tratamento de erros."""
    try:
        return json.loads(data_str)
    except Exception:
        return None
