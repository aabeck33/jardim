import time
import threading
#import keyboard
from pynput import mouse, keyboard


# Dicionário para armazenar o último "batimento" de cada thread
heartbeat = {"teclado": 0, "mouse": 0}
heartbeat_timeout = 10
heartbeat_lock = threading.Lock()
# Flags de parada
stop_flags = {"teclado": threading.Event(), "mouse": threading.Event()}
# Contador de reinicializações
restart_count = {"teclado": 0, "mouse": 0}


def listar_threads():
    print("\n=== Threads Python ativas ===")
    for t in threading.enumerate():
        print(f"Nome: {t.name}, ID: {t.ident}")
    print("=============================\n")


def keyboard_listen(stop_event):
    print("[Teclado] Thread iniciada.")

    def on_press(key):
        try:
            print(f"[Teclado] Tecla {key.char} pressionada!")
        except AttributeError:
            print(f"[Teclado] Tecla especial {key} pressionada!")

        # Atualiza heartbeat sempre que há evento
        with heartbeat_lock:
            heartbeat["teclado"] = time.time()

    listener = keyboard.Listener(on_press=on_press)
    listener.start()

    # Mantém thread ativa até pedir para parar
    try:
        while not stop_event.is_set():
            time.sleep(0.2)
            with heartbeat_lock:
                heartbeat["teclado"] = time.time()
    finally:
        listener.stop()
        listener.join()
        print("[Teclado] Thread finalizada com sucesso.")


def mouse_listen(stop_event):
    print("[Mouse] Thread iniciada.")

    def on_click(x, y, button, pressed):
        if pressed:
            print(f"[Mouse] Clique detectado {button} em ({x}, {y})")
            # Chame sua função aqui
        with heartbeat_lock:
            heartbeat["mouse"] = time.time()

    listener = mouse.Listener(on_click=on_click)
    listener.start()

    try:
        while not stop_event.is_set():
            with heartbeat_lock:
                heartbeat["mouse"] = time.time()
            time.sleep(0.2)
    finally:
        print("[Mouse] Sinal de parada recebido.")
        listener.stop()
        listener.join()
        print("[Mouse] Thread finalizada com sucesso.")


def start_thread(target_func, name, stop_event):
    t = threading.Thread(target=target_func, args=(stop_event,), daemon=True, name=name)
    t.start()
    return t


def restart_thread(name, target_func, old_thread, stop_event_key):
    print(f"[{name}] Reiniciando...")

    # pede para parar
    stop_flags[stop_event_key].set()
    old_thread.join(timeout=15)

    if old_thread.is_alive():
        print(f"[{name}] ⚠️ Thread não finalizou no tempo esperado!")

    restart_count[stop_event_key] += 1
    print(f"[{name}] Reinicializações: {restart_count[stop_event_key]}")

    # cria nova flag
    stop_flags[stop_event_key] = threading.Event()

    # inicia nova thread
    new_thread = start_thread(target_func, f"{name}_aaBeck", stop_flags[stop_event_key])

    with heartbeat_lock:
        heartbeat[stop_event_key] = time.time()

    return new_thread


if __name__ == "__main__":
    print("[Main] Thread iniciada.")
    # Inicializa os heartbeats
    heartbeat["teclado"] = time.time()
    heartbeat["mouse"] = time.time()
    # Inicia as threads de monitoramento
    t1 = start_thread(keyboard_listen, "Teclado_aaBeck", stop_flags["teclado"])
    t2 = start_thread(mouse_listen, "Mouse_aaBeck", stop_flags["mouse"])
    time.sleep(5)
    listar_threads()

    try:
        while True:
            now = time.time()

            if now - heartbeat["teclado"] > heartbeat_timeout or not t1.is_alive():
                t1 = restart_thread("Teclado", keyboard_listen, t1, "teclado")
            if now - heartbeat["mouse"] > heartbeat_timeout or not t2.is_alive():
                t2 = restart_thread("Mouse", mouse_listen, t2, "mouse")

            time.sleep(5)

    except KeyboardInterrupt:
        print("\n[Main] Encerrando todas as threads...")
        stop_flags["teclado"].set()
        stop_flags["mouse"].set()
        t1.join(timeout=3)
        t2.join(timeout=3)
        print("[Main] Finalizado com segurança.")
