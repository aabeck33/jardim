# Coding Rules and Standards

## C++ Coding Style
- Use **CamelCase** for functions and variables.
- Keep functions small and focused on a single task.
- Use meaningful variable names (e.g., `sensorValue` instead of `val`).
- Avoid blocking calls (`delay()`) unless necessary; use non-blocking timing where possible.

## Project Specifics
- Always check LoRa initialization status before transmission.
- Ensure OLED updates are handled efficiently to avoid flickering.
- Use `Serial.println()` for debugging, but consider disabling it in production builds.

## Agent Behavior
- When suggesting code changes, explain the logic behind the implementation.
- If modifying `platformio.ini`, ensure dependencies are compatible.
