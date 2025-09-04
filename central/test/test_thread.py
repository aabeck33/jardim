import time
import threading
import keyboard
from pynput import mouse


# Dicionário para armazenar o último "batimento" de cada thread
heartbeat = {"teclado": 0, "mouse": 0}
heartbeat_timeout = 10
heartbeat_lock = threading.Lock()
# Flags de parada
stop_flags = {"teclado": threading.Event(), "mouse": threading.Event()}



def listar_threads():
    print("Threads Python ativas:")
    for t in threading.enumerate():
        print(f"Nome: {t.name}, ID: {t.ident}")


def keyboard_listen(stop_event):
    print("Pressione qualquer tecla para executar a função.")

    def on_key(event):
        if event.event_type == keyboard.KEY_DOWN:
            print(f"Tecla {event.name} pressionada! Executando função...")
            # Aqui você chama sua função
        # Atualiza heartbeat sempre que há evento
        with heartbeat_lock:
            heartbeat["teclado"] = time.time()

    keyboard.hook(on_key)

    # Mantém thread ativa até pedir para parar
    while not stop_event.is_set():
        with heartbeat_lock:
            heartbeat["teclado"] = time.time()
        time.sleep(0.2)

    keyboard.unhook_all()


def mouse_listen(stop_event):
    def on_click(x, y, button, pressed):
        if pressed:
            print(f"Mouse {button} pressionado em ({x}, {y})")
            # Chame sua função aqui

    with mouse.Listener(on_click=on_click) as listener:
        while not stop_event.is_set():
            with heartbeat_lock:
                heartbeat["mouse"] = time.time()
            time.sleep(0.2)


def start_thread(target_func, name, stop_event):
    t = threading.Thread(target=target_func, args=(stop_event,), daemon=True, name=name)
    t.start()
    return t


if __name__ == "__main__":
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
                stop_flags["teclado"].set()
                t1.join(timeout=2)
                stop_flags["teclado"] = threading.Event()
                t1 = start_thread(keyboard_listen, "Teclado_aaBeck", stop_flags["teclado"])
                with heartbeat_lock:
                    heartbeat["teclado"] = time.time()
            if now - heartbeat["mouse"] > heartbeat_timeout or not t2.is_alive():
                stop_flags["mouse"].set()
                t2.join(timeout=2)
                stop_flags["mouse"] = threading.Event()
                t2 = start_thread(mouse_listen, "Mouse_aaBeck", stop_flags["mouse"])
                with heartbeat_lock:
                    heartbeat["mouse"] = time.time()
            time.sleep(5)
    except KeyboardInterrupt:
        stop_flags["teclado"].set()
        stop_flags["mouse"].set()
        t1.join(timeout=2)
        t2.join(timeout=2)
