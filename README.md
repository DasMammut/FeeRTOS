FeeRTOS - Technical Reference

FeeRTOS ist ein minimalistischer, präemptiver Echtzeit-Kernel, der für deterministische Performance auf ressourcenbeschränkten Mikrocontrollern (Fokus: AVR/ARM) entwickelt wurde. Die Architektur folgt einem strikten Layer-Modell zur Trennung von Kernel-Logik und Hardware-Abstraktion (Port-Layer).
1. System-Kern & Task-Management

Das Herzstück des Kernels verwaltet den CPU-Kontext und das Scheduling.

    Präemptives Scheduling: Unterstützt Round-Robin bei gleicher Priorität sowie sofortiges Preemption bei höherpriorisierten Tasks.

    Task-Management: Funktionen zum Erstellen (Create), Löschen (Delete), Suspendieren und Resumieren von Tasks.

    Idle-Task: Ein automatisch generierter Task mit niedrigster Priorität, der das System in den Schlafmodus versetzt, wenn keine Arbeit ansteht.

    Context Switching: Hardware-optimiertes Sichern und Wiederherstellen der CPU-Register über einen dedizierten Port-Layer.

2. Synchronisation & Inter-Process Communication (IPC)

FeeRTOS bietet eine breite Palette an Mechanismen zur Thread-Sicherheit und Kommunikation.
Mutexe & Semaphoren

    Mutexe: Binäre Sperren für den exklusiven Ressourcenzugriff. Implementiert Priority Inheritance, um das Problem der Prioritätsinversion zu lösen.

    Rekursive Mutexe: Ermöglicht einem Besitzer, den Mutex mehrfach zu sperren (Reentrancy).

    Zählende Semaphoren: Zur Verwaltung von Ressourcen-Pools oder zur Signalisierung von Ereignissen zwischen Tasks/ISRs.

Kommunikation

    Queues: Thread-sichere FIFO-Datenströme für den Austausch von Objekten zwischen Tasks.

    Mailboxen: Spezialisierte 1-Element-Queues für effizientes Messaging und Status-Updates.

    Ringbuffer: Hochperformante, lock-freie (bei Single-Producer/Single-Consumer) oder gesperrte Puffer für Byte-Streams (z.B. UART-Handling).

Event Groups

    Event-Bits: Tasks können auf die Kombination mehrerer Bits (Flags) warten.

    Logische Verknüpfungen: Unterstützt Wait_AND (alle Bits gesetzt) und Wait_OR (mindestens ein Bit gesetzt) Bedingungen.

3. Zeit- & Timer-Management

    Software-Timer (SWTimer): Ermöglicht das Ausführen von Callbacks nach Ablauf einer definierten Zeitspanne (One-Shot oder Periodisch).

    Task-Delay: Blockiert den aufrufenden Task für eine exakte Anzahl von System-Ticks, ohne CPU-Zyklen zu verschwenden.

4. Speicher- & Stack-Management

Da embedded Systeme kritisch auf Speicherfehler reagieren, bietet FeeRTOS spezialisierte Manager:

    Heap-Strategien:

        Heap 1: Unterstützt Allokation und Deallokation mit einem Best-Fit-Algorithmus für dynamischere Anforderungen. (Header-Block groß)

        Heap 2: Unterstützt Allokation und Deallokation mit einem Best-Fit-Algorithmus für dynamischere Anforderungen.

    Stack-Management: Jeder Task erhält einen isolierten Stack-Bereich. Der Port-Layer initialisiert diesen so, dass er wie ein unterbrochener Interrupt-Kontext aussieht.

