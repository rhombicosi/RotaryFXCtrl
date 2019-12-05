class entity {
    constructor(position, color) {
        this.position = position || {x: 0, y: 0}
        this.color = color || 'green'
    }
}

module.exports = entity