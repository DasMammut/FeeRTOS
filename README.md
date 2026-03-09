FeeRTOS 🚀
Ein leichtgewichtiges, präemptives Echtzeit-Betriebssystem (RTOS) für AVR-Mikrocontroller.

FeeRTOS ist ein moderner Echtzeit-Kernel, der speziell für Systeme mit begrenzten Ressourcen (wie den ATmega4809) entwickelt wurde. Es bietet die perfekte Balance zwischen minimalem Speicherverbrauch und professionellen Features, die normalerweise nur in großen Systemen wie FreeRTOS zu finden sind.

✨ Kern-Features
Präemptives Scheduling: Prioritätsbasiertes Multitasking sorgt dafür, dass der wichtigste Task immer sofort die CPU erhält.

Prioritätsvererbung (Priority Inheritance): Ein entscheidendes Sicherheitsfeature für Mutexe. Es verhindert, dass hochpriorisierte Tasks durch niedrigere blockiert werden (Priority Inversion).

Event Groups: Ermöglicht Tasks das Warten auf komplexe Bit-Kombinationen (AND/OR-Verknüpfungen) – ideal für die Synchronisation mehrerer Ereignisse.

Zählende Semaphoren: Zur effizienten Verwaltung von Ressourcen und zur Signalisierung zwischen Tasks.

Rekursive Mutexe: Erlaubt es einem Task, dieselbe Ressource mehrfach zu sperren, ohne sich selbst zu blockieren.

🛠 Architektur & Sicherheit
Schutz vor Priority Inversion
Im Gegensatz zu einfachen Kernels implementiert FeeRTOS eine dynamische Prioritätsanpassung. Hält ein niedriger Task einen Mutex, den ein hoher Task benötigt, wird die Priorität des niedrigen Tasks temporär angehoben. Dies garantiert deterministische Antwortzeiten in komplexen Systemen.

Effizientes Event-Management
Die Event Groups in FeeRTOS sind bit-optimiert. Ein einziger Aufruf kann mehrere wartende Tasks gleichzeitig aufwecken, was den Overhead im Vergleich zu vielen einzelnen Semaphoren massiv reduziert.
