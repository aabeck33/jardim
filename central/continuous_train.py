import pandas as pd
import joblib
from sklearn.linear_model import SGDClassifier, SGDRegressor
from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline
import os

CSV_FILE = "dados_irrigacao.csv"
MODEL_CLASS_FILE = "modelo_class.pkl"
MODEL_REG_FILE = "modelo_reg.pkl"

def load_data():
    df = pd.read_csv(CSV_FILE)
    X = df[["soil_moisture", "airTemperature", "cloudCover", "humidity", "precipitation", "waterTemperature", "windSpeed"]]
    y_class = df["irrigar"]
    y_reg = df["tempo_bomba"]
    return X, y_class, y_reg

def train_or_update():
    X, y_class, y_reg = load_data()

    if os.path.exists(MODEL_CLASS_FILE) and os.path.exists(MODEL_REG_FILE):
        print("🔄 Atualizando modelos existentes...")
        clf = joblib.load(MODEL_CLASS_FILE)
        reg = joblib.load(MODEL_REG_FILE)

        clf.partial_fit(X, y_class, classes=[0,1])
        reg.partial_fit(X, y_reg)

    else:
        print("⚡ Treinando modelos novos...")
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
    train_or_update()
    print("Modelos treinados e salvos.")
