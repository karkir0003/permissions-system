package permissions;

public enum Action {
    READ(1),
    REQUEST(2),
    WRITE(3);

    private final int level;

    Action(int level) {
        this.level = level;
    }

    public int getLevel() {
        return level;
    }

    public boolean subsumes(Action requested) {
        return this == requested;
    }
}
