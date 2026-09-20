from pathlib import Path
import pandas as pd
import joblib
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier, RandomForestRegressor
from sklearn.linear_model import SGDClassifier, SGDRegressor
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline
from config import settings as cfg

BASE_DIR = Path(__file__).resolve().parent
CSV_FILE = BASE_DIR / "data" / "dados_irrigacao.csv"
MODEL_CLASS_FILE = BASE_DIR / "models" / "modelo_class.pkl"
MODEL_REG_FILE = BASE_DIR / "models" / "modelo_reg.pkl"


def load_data():
    if not CSV_FILE.exists():
        raise FileNotFoundError(f"Arquivo CSV de dados não encontrado em {CSV_FILE}")
    df = pd.read_csv(CSV_FILE)
    X = df[["soil_moisture", "airTemperature", "cloudCover", "humidity", "precipitation", "waterTemperature", "windSpeed"]]
    y_class = df["irrigar"]
    y_reg = df["tempo_bomba"]
    return X, y_class, y_reg


def train_initial_models():
    """Treina os modelos do zero usando RandomForest."""
    print("⚡ Treinando modelos RandomForest do zero...")
    X, y_class, y_reg = load_data()

    X_train, X_test, y_class_train, y_class_test = train_test_split(X, y_class, test_size=0.2, random_state=42)
    X_train_r, X_test_r, y_reg_train, y_reg_test = train_test_split(X, y_reg, test_size=0.2, random_state=42)

    clf = RandomForestClassifier(n_estimators=250, random_state=42)
    reg = RandomForestRegressor(n_estimators=250, random_state=42)

    clf.fit(X_train, y_class_train)
    reg.fit(X_train_r, y_reg_train)

    if cfg.DEBUG_MODE:
        print("Acurácia (irrigar ou não):", clf.score(X_test, y_class_test))
        print("R² (tempo bomba):", reg.score(X_test_r, y_reg_test))

    MODEL_CLASS_FILE.parent.mkdir(parents=True, exist_ok=True)
    joblib.dump(clf, MODEL_CLASS_FILE)
    joblib.dump(reg, MODEL_REG_FILE)
    print("✅ Modelos treinados e salvos com sucesso!")


def train_or_update_continuous():
    """Atualização contínua de modelos usando SGD."""
    X, y_class, y_reg = load_data()

    if MODEL_CLASS_FILE.exists() and MODEL_REG_FILE.exists():
        print("🔄 Atualizando modelos existentes...")
        clf = joblib.load(MODEL_CLASS_FILE)
        reg = joblib.load(MODEL_REG_FILE)

        if hasattr(clf, "partial_fit") and hasattr(reg, "partial_fit"):
            clf.partial_fit(X, y_class, classes=[0, 1])
            reg.partial_fit(X, y_reg)
        else:
            print("⚠️ Modelo existente não suporta partial_fit. Re-treinando...")
            train_initial_models()
            return
    else:
        print("⚡ Criando e treinando novos modelos SGD...")
        clf = Pipeline([
            ('scaler', StandardScaler()),
            ('clf', SGDClassifier(max_iter=1000, tol=1e-3))
        ])
        reg = Pipeline([
            ('scaler', StandardScaler()),
            ('reg', SGDRegressor(max_iter=1000, tol=1e-3))
        ])

        clf.fit(X, y_class)
        reg.fit(X, y_reg)

    joblib.dump(clf, MODEL_CLASS_FILE)
    joblib.dump(reg, MODEL_REG_FILE)
    print("✅ Modelos salvos/atualizados com sucesso!")


if __name__ == "__main__":
    train_initial_models()
