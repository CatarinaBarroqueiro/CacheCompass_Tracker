import { Column, Entity, JoinColumn, JoinTable, ManyToMany, OneToOne, PrimaryColumn } from 'typeorm';

@Entity({ name: 'User' })
export class User {
    @PrimaryColumn()
    public idUser: string;

    @Column()
    public name: string;

    @Column()
    public email: string;

    @Column()
    public flag: boolean;

    @Column()
    public password: string;

    @Column()
    public admin: boolean;

}