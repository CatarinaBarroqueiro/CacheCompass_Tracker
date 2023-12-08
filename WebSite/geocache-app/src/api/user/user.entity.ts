import { Column, Entity, JoinColumn, JoinTable, ManyToMany, OneToOne, PrimaryColumn, PrimaryGeneratedColumn } from 'typeorm';

@Entity({ name: 'User' })
export class User {
    @PrimaryGeneratedColumn()
    public idUser: number;

    @Column( {nullable:false})
    public name: string;

    @Column({nullable:false})
    public email: string;

    @Column()
    public flag: boolean;

    @Column({nullable:false})
    public password: string;

    @Column()
    public admin: boolean;

}