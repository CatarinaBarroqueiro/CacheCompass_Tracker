import { Column, Entity, JoinColumn, JoinTable, ManyToMany, OneToOne, PrimaryGeneratedColumn } from 'typeorm';

@Entity({ name: 'Box' })
export class Box {
    @PrimaryGeneratedColumn()
    public idBox: number;

    @Column()
    public local: string;

}