# TODO

- Klassendiagramm: IActuatorDrive ist inkonsistent modelliert. Korrekt ist apply(cmd): bool; die veralteten Methoden extend(), retract(), stop() im Diagramm entfernen.
- Sequenzdiagramm mqtt_state_publish: Rueckgabe ok : bool nach publishState(state) entfernen; publishState ist bewusst void.
- State/Sequenz Rueckhub nach Erfolg: Top-Endstop-Pfad fuer RETURN_UP_SUCCESS explizit und konsistent darstellen, damit kein Hanger entsteht und die endstop_reached-Sequenz eindeutig passt.
