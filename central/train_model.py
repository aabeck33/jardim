import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier, RandomForestRegressor
import joblib

# Carregar dados
df = pd.read_csv("dados_irrigacao.csv")

# Features e targets
X = df[["soil_moisture", "airTemperature", "cloudCover", "humidity", "precipitation", "waterTemperature", "windSpeed"]]
y_class = df["irrigar"]       # classificação: irriga ou não
y_reg = df["tempo_bomba"]     # regressão: tempo de bomba

# Divisão treino/teste
X_train, X_test, y_class_train, y_class_test = train_test_split(X, y_class, test_size=0.2, random_state=42)
X_train_r, X_test_r, y_reg_train, y_reg_test = train_test_split(X, y_reg, test_size=0.2, random_state=42)

# Modelos
clf = RandomForestClassifier(n_estimators=250, random_state=42)
reg = RandomForestRegressor(n_estimators=250, random_state=42)

# Treino
clf.fit(X_train, y_class_train)
reg.fit(X_train_r, y_reg_train)

# Avaliação rápida
print("Acurácia (irrigar ou não):", clf.score(X_test, y_class_test))
print("R² (tempo bomba):", reg.score(X_test_r, y_reg_test))

# Salvar modelos
joblib.dump(clf, "modelo_class.pkl")
joblib.dump(reg, "modelo_reg.pkl")
print("Modelos treinados e salvos.")
