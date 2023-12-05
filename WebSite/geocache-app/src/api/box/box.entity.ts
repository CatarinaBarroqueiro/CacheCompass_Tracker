import { Column, Entity, JoinColumn, JoinTable, ManyToMany, OneToOne, PrimaryColumn } from 'typeorm';

@Entity({ name: 'Box' })
export class Box {
    @PrimaryColumn()
    public idBox: string;

    @Column()
    public local: string;

}