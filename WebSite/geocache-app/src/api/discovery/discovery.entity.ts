import { Column, Entity, JoinColumn, JoinTable, ManyToMany, OneToOne, PrimaryColumn } from 'typeorm';
import { User } from '../user/user.entity';
import { Box } from '../box/box.entity';

@Entity({ name: 'Discovery' })
export class Discovery {
    @PrimaryColumn()
    public idDiscovery: string;

    @ManyToMany(() => Box)
    @JoinColumn({ name: 'idBox' })
    public id_box: string;

    @ManyToMany(() => User)
    @JoinColumn({ name: 'idUser' })
    public id_user: string;

    @Column({ default: () => 'CURRENT_TIMESTAMP' })
    discTime: string;

    @Column()
    public authorized: boolean;


}