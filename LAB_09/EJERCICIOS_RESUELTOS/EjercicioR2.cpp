#include <iostream>
#include <map>
#include <string>
#include <memory>
using namespace std;

/** Bullet is the base Prototype */
class Bullet {
protected:
    string _bulletName;
    float _speed = 0;
    float _firePower = 0;
    float _damagePower = 0;
    float _direction = 0;                         // la guia lo dejaba sin inicializar y luego lo copiaba al clonar

public:
    Bullet() {}
    Bullet(string bulletName, float speed, float firePower, float damagePower)
        : _bulletName(bulletName), _speed(speed), _firePower(firePower), _damagePower(damagePower) {}
    Bullet(const Bullet&) = default;              // la copia de todos los miembros es lo que usa clone()
    virtual ~Bullet() {}
    virtual unique_ptr<Bullet> clone() const = 0; // const: clonar no modifica el prototipo
    void fire(float direction) {
        _direction = direction;
        cout << "Name        : " << _bulletName << endl
             << "Speed       : " << _speed << endl
             << "FirePower   : " << _firePower << endl
             << "DamagePower : " << _damagePower << endl
             << "Direction   : " << _direction << endl << endl;
    }
};

class SimpleBullet : public Bullet {
public:
    SimpleBullet(string bulletName, float speed, float firePower, float damagePower)
        : Bullet(bulletName, speed, firePower, damagePower) {}
    unique_ptr<Bullet> clone() const override { return make_unique<SimpleBullet>(*this); }
};

/** GoodBullet is the Concrete Prototype */
class GoodBullet : public Bullet {
public:
    GoodBullet(string bulletName, float speed, float firePower, float damagePower)
        : Bullet(bulletName, speed, firePower, damagePower) {}
    unique_ptr<Bullet> clone() const override { return make_unique<GoodBullet>(*this); }
};

enum class BulletType { SIMPLE, GOOD };

class BulletFactory {
private:
    map<BulletType, unique_ptr<Bullet>> m_Bullets;

public:
    BulletFactory() {
        m_Bullets[BulletType::SIMPLE] = make_unique<SimpleBullet>("Simple Bullet", 50, 75, 75);
        m_Bullets[BulletType::GOOD]   = make_unique<GoodBullet>("Good Bullet", 75, 100, 100);
    }

    unique_ptr<Bullet> createBullet(BulletType type) const {
        auto it = m_Bullets.find(type);           // la guia usaba operator[], que inserta un nulo si falta la clave
        if (it == m_Bullets.end()) return nullptr;
        return it->second->clone();
    }
};

int main() {
    BulletFactory bulletFactory;

    auto bullet = bulletFactory.createBullet(BulletType::SIMPLE);
    bullet->fire(90);

    bullet = bulletFactory.createBullet(BulletType::GOOD);
    bullet->fire(100);

    // Cada clon es independiente del prototipo: un segundo disparo del tipo SIMPLE no hereda la direccion anterior
    auto otra = bulletFactory.createBullet(BulletType::SIMPLE);
    otra->fire(45);
    return 0;
}