"""
Painel de Controle de Irrigação Inteligente - Streamlit App
Execução: streamlit run pages/app.py
"""
import streamlit as st
import pandas as pd
from pathlib import Path

st.set_page_config(
    page_title="Central de Irrigação Inteligente",
    page_icon="🌱",
    layout="wide"
)

st.title("🌱 Painel de Controle - Irrigação Inteligente")
st.markdown("### Monitoramento em Tempo Real e Controle do Sistema")

st.info("Interface Streamlit pronta para integração com os dados dos sensores e previsões de IA.")

col1, col2, col3 = st.columns(3)

with col1:
    st.metric(label="Status da Bomba", value="Desligada", delta="Seguro")

with col2:
    st.metric(label="Última Umidade lida", value="45%", delta="-2%")

with col3:
    st.metric(label="Previsão de Chuva", value="0.0 mm", delta="0 mm")

st.subheader("📊 Histórico Recente de Leituras")
st.caption("Visualização simplificada dos dados de sensores e modelo de decisão.")

# Exemplo mock de tabela
df_mock = pd.DataFrame({
    "Data/Hora": ["2026-09-20 14:00", "2026-09-20 14:05", "2026-09-20 14:10"],
    "Umidade Solo (%)": [42, 40, 45],
    "Temperatura (°C)": [26.5, 27.0, 26.8],
    "Irrigado?": ["Não", "Sim (12s)", "Não"]
})

st.dataframe(df_mock, use_container_width=True)
