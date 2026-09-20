# Design

## Data Model


### Organisation Data Model

- Foundation
  - budget
  - mtfs
  - scps
  - reputation
    - high/medium/low
- SCP
  - ID
  - name
  - object class
  - description
- MTF
  - name
  - member count
    - operational
    - wounded
    - dead
  - specialism

### Rooms Data Model

- Room
  - name
  - ID
  - description
  - characters
  - objects
  - connections
  - flag(s) required
- Facility
  - rooms
  - entrances
- Containment
  - ID
  - name
  - scp ID
  - description
  - flag(s) required

### Character Data Model

- Character
  - ID
  - name
  - description
  - role
  - conversations
  - flag(s) required

- Conversation
  - ID
  - nodes
    - ID
    - text
    - options
- ConversationOption
  - text
  - links to
  - flag(s) required
  - flag(s) set

### Narrative

- Narrative beats will be represented by flags
    - The flags will exist in an integer value, with each flag being a bit on that integer
    - Conversations, interactions, and so on will be able to set (or unset) flag(s) based on the position of the bit that needs (un)set
    - They will also be able to be locked behind flag(s)

## Components


