import time
import threading
import keyboard
from pynput import mouse


def listar_threads():
    print("Threads Python ativas:")
    for t in threading.enumerate():
        print(f"Nome: {t.name}, ID: {t.ident}")


def keyboard_listen():
    print("Pressione qualquer tecla para executar a função.")
    while True:
        event = keyboard.read_event()
        if event.event_type == keyboard.KEY_DOWN:
            print("Tecla pressionada! Executando função...")
            # Chame sua função aqui
            time.sleep(0.2)  # Evita múltiplos triggers

def mouse_listen():
    def on_click(x, y, button, pressed):
        if pressed:
            print(f"Mouse {button} pressionado em ({x}, {y})")
            # Chame sua função aqui

    with mouse.Listener(on_click=on_click) as listener:
        listener.join()

if __name__ == "__main__":

    def start_thread(target_func, name, *args):
        t = threading.Thread(target=target_func, args=args, daemon=True)
        t.start()
        return t

    t1 = start_thread(keyboard_listen, "Teclado_aaBeck")
    t2 = start_thread(mouse_listen, "Mouse_aaBeck")
    listar_threads()

    try:
        while True:
            if not t1.is_alive():
                print("Thread teclado caiu! Reiniciando...")
                t1 = start_thread(keyboard_listen, "Teclado_aaBeck")
                listar_threads()
            if not t2.is_alive():
                print("Thread mouse caiu! Reiniciando...")
                t2 = start_thread(mouse_listen, "Mouse_aaBeck")
                listar_threads()
            time.sleep(1)
    except KeyboardInterrupt:
        print("Encerrando programa.")
